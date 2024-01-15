#include "data/button.h"
#include "data/ring_buffer.h"
#include "hardware/gpio.h"
#include "hardware/hw_serial.h"
#include "hardware/rotary_encoder.h"
#include "hardware/segment_display.h"
#include "hardware/shift_register.h"
#include "hardware/spi.h"
#include "hardware/sw_serial.h"
#include "hardware/timer0.h"
#include "logging.h"
#include "sequencer/beat_clock.h"
#include "user_interface/playback_control.h"
#include "user_interface/ui_devices.h"
#include "util/bits.h"
#include "util/math.h"
#include "util/usec_timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define DEFAULT_BPM 120
#define QUARTERNOTE_PULSE_LENGTH_US 500

#define MIDI_CLOCK_BYTE 0xF8
#define MIDI_START_BYTE 0xFA
#define MIDI_CONTINUE_BYTE 0xFB
#define MIDI_STOP_BYTE 0xFC

/* ----------------------- Interrupt service routines ----------------------- */
ISR(TIMER0_OVF_vect) {
	timer0_timer_overflow_irq();
}

ISR(PCINT2_vect) {
	sw_serial_pin_change_irq();
}

ISR(USART_RX_vect) {
	hw_serial_rx_complete_irq();
}

ISR(USART_UDRE_vect) {
	hw_serial_tx_udr_empty_irq();
}

/* ------------------------------ Main Program ------------------------------ */
static void globally_enable_interrupts(void) {
	sei();
}

static void update_button_states(button_t* buttons, uint8_t num_buttons, const shift_register_t* shift_reg) {
	bool button_input[256];
	shift_register_read(shift_reg, button_input, num_buttons);
	for (uint8_t i = 0; i < num_buttons; i++) {
		button_update(&buttons[i], button_input[i], timer0_now_ms());
	}
}

static uint8_t read_midi_byte(void) {
	uint8_t midi_byte = 0;
	if (sw_serial_available_bytes() > 0) {
		sw_serial_read(&midi_byte);
	}
	return midi_byte;
}

int main(void) {
	/* Setup */
	const gpio_pin_t start_button_pin = gpio_pin_init(&PORTC, 3);
	gpio_pin_configure(start_button_pin, PIN_MODE_INPUT);
	const gpio_pin_t pulse_pin = gpio_pin_init(&PORTC, 5);
	gpio_pin_configure(pulse_pin, PIN_MODE_OUTPUT);
	const gpio_pin_t midi_rx_pin = gpio_pin_init(&PORTD, 2);
	const gpio_pin_t midi_tx_pin = gpio_pin_init(&PORTD, 3);
	const gpio_pin_t encoder_a_pin = gpio_pin_init(&PORTD, 4);
	const gpio_pin_t encoder_b_pin = gpio_pin_init(&PORTD, 5);
	const gpio_pin_t display_data_pin = gpio_pin_init(&PORTD, 6);
	const gpio_pin_t display_latch_pin = gpio_pin_init(&PORTD, 7);
	const gpio_pin_t display_clock_pin = gpio_pin_init(&PORTB, 0);
	const gpio_pin_t step_buttons_latch_pin = gpio_pin_init(&PORTB, 1);
	const gpio_pin_t step_leds_latch_pin = gpio_pin_init(&PORTB, 2);

	globally_enable_interrupts();
	timer0_initialize();
	hw_serial_initialize(9600); // uses PD0 and PD1
	sw_serial_initialize(31250, midi_rx_pin, midi_tx_pin);
	spi_t spi = spi_initialize(SPI_DATA_ORDER_MSB_FIRST); // uses PB3, PB4 and PB5
	shift_register_t step_buttons_shift_reg = shift_register_init(spi, step_buttons_latch_pin);
	shift_register_t step_leds_shift_reg = shift_register_init(spi, step_leds_latch_pin);
	ui_devices_t ui_devices = {
		.start_button = { 0 },
		.step_buttons = { 0 },
		.encoder = rotary_encoder_init(encoder_a_pin, encoder_b_pin),
		.display = segment_display_init(display_clock_pin, display_latch_pin, display_data_pin),
	};
	beat_clock_t beat_clock = beat_clock_init(DEFAULT_BPM);
	usec_timer_t pulse_timer = usec_timer_init(QUARTERNOTE_PULSE_LENGTH_US);

	uint8_t led_state = 0;

	/* Run */
	const uint64_t ppqn_period = (60 * 1e6) / (DEFAULT_BPM * 24);
	usec_timer_t midi_ppqn_timer = usec_timer_init(ppqn_period);

	bool playback_started = false;
	uint8_t midi_clock_pulses = 0;
	LOG_INFO("Program Start\n");
	while (true) {
		/* Input */
		update_button_states(ui_devices.step_buttons, 8, &step_buttons_shift_reg);
		button_update(&ui_devices.start_button, gpio_pin_read(start_button_pin), timer0_now_ms());
		segment_display_update(&ui_devices.display); // cycle to next digit
		const uint8_t midi_byte = read_midi_byte();

		/* Update */
		beat_clock_update(&beat_clock);
		playback_control_update(&ui_devices, &beat_clock);
		led_state = button_is_pressed(&ui_devices.step_buttons[0]) ? 0xFF : 0x0;

		// Proof of concept MIDI handling
		switch (midi_byte) {
			case MIDI_CLOCK_BYTE:
				if (playback_started) {
					midi_clock_pulses++;
				}
				break;

			case MIDI_START_BYTE:
				playback_started = true;
				midi_clock_pulses = 23; // trigger note on next clock pulse
				break;

			case MIDI_CONTINUE_BYTE:
				playback_started = true;
				break;

			case MIDI_STOP_BYTE:
				playback_started = false;
				break;
		}

		/* Output */
		if (beat_clock_quarternote_ready(&beat_clock)) {
			gpio_pin_set(pulse_pin);
			usec_timer_reset(&pulse_timer);
		}
		if (usec_timer_period_has_elapsed(&pulse_timer)) {
			gpio_pin_clear(pulse_pin);
		}

		// Step leds
		shift_register_write(&step_leds_shift_reg, &led_state, 1);

		// Proof of concept MIDI clock input
		if (midi_clock_pulses == 24) {
			LOG_INFO("Tick\n");
			midi_clock_pulses = 0;
		}

		// Proof of concept MIDI clock output
		// PROBLEM:
		// Just using usec_timer is NOT precise enough to guarantee an accurate
		// enough BPM. Propper implementation will require using interrupts of a
		// high precision hardware timer.
		// Probably timer1 (16-bit timer) will be adequate.
		if (usec_timer_period_has_elapsed(&midi_ppqn_timer)) {
			usec_timer_reset(&midi_ppqn_timer);
			sw_serial_write(MIDI_CLOCK_BYTE);
		}
	}
}
