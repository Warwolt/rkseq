#include "data/button.h"
#include "data/ring_buffer.h"
#include "hardware/gpio.h"
#include "hardware/hw_serial.h"
#include "hardware/rotary_encoder.h"
#include "hardware/segment_display.h"
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
void globally_enable_interrupts(void) {
	sei();
}

int main(void) {
	/* Setup */
	const gpio_pin_t pulse_pin = gpio_pin_init(&PORTC, 5);
	const gpio_pin_t midi_rx_pin = gpio_pin_init(&PORTD, 2);
	const gpio_pin_t midi_tx_pin = gpio_pin_init(&PORTD, 3);
	const gpio_pin_t encoder_a_pin = gpio_pin_init(&PORTD, 4);
	const gpio_pin_t encoder_b_pin = gpio_pin_init(&PORTD, 5);
	const gpio_pin_t display_clock_pin = gpio_pin_init(&PORTD, 6);
	const gpio_pin_t display_latch_pin = gpio_pin_init(&PORTD, 7);
	const gpio_pin_t display_data_pin = gpio_pin_init(&PORTB, 0);
	const gpio_pin_t start_button_pin = gpio_pin_init(&PORTC, 4);

	// shift reg buttons
	const gpio_pin_t step_buttons_parallel_load_pin = gpio_pin_init(&PORTB, 1);
	const gpio_pin_t step_buttons_clock_enable_pin = gpio_pin_init(&PORTB, 2);
	gpio_pin_configure(step_buttons_parallel_load_pin, PIN_MODE_OUTPUT);
	gpio_pin_configure(step_buttons_clock_enable_pin, PIN_MODE_OUTPUT);

	globally_enable_interrupts();
	timer0_initialize();
	hw_serial_initialize(9600); // uses PD0 and PD1
	sw_serial_initialize(31250, midi_rx_pin, midi_tx_pin);
	gpio_pin_configure(pulse_pin, PIN_MODE_OUTPUT);
	gpio_pin_configure(start_button_pin, PIN_MODE_INPUT);
	spi_initialize(SPI_DATA_ORDER_MSB_FIRST); // uses PB3, PB4 and PB5

	ui_devices_t ui_devices = {
		.start_button = button_init(),
		.encoder = rotary_encoder_init(encoder_a_pin, encoder_b_pin),
		.display = segment_display_init(display_clock_pin, display_latch_pin, display_data_pin),
	};
	beat_clock_t beat_clock = beat_clock_init(DEFAULT_BPM);
	usec_timer_t pulse_timer = usec_timer_init(QUARTERNOTE_PULSE_LENGTH_US);

	/* Run */
	uint32_t last_print_time = 0;
	LOG_INFO("Program Start\n");
	while (true) {
		/* Update devices */
		button_update(&ui_devices.start_button, gpio_pin_read(start_button_pin), timer0_now_ms());
		segment_display_update(&ui_devices.display); // cycle to next digit

		/* Update sequencer playback */
		beat_clock_update(&beat_clock);
		playback_control_update(&ui_devices, &beat_clock);

		/* Output tempo pulse */
		if (beat_clock_should_output_quarternote(&beat_clock)) {
			gpio_pin_set(pulse_pin);
			usec_timer_reset(&pulse_timer);
		}
		if (usec_timer_period_has_elapsed(&pulse_timer)) {
			gpio_pin_clear(pulse_pin);
		}

		// load button states into shift register
		gpio_pin_clear(step_buttons_parallel_load_pin);
		gpio_pin_set(step_buttons_parallel_load_pin);

		// enable shift register SPI and read states
		gpio_pin_clear(step_buttons_clock_enable_pin);
		const uint8_t step_buttons_state = spi_receive();
		gpio_pin_set(step_buttons_clock_enable_pin);

		const uint32_t now_ms = timer0_now_ms();
		if (now_ms - last_print_time >= 100) {
			last_print_time = now_ms;
			LOG_INFO("button state = %x\n", step_buttons_state & 1);
		}
	}
}
