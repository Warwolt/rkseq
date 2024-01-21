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
#include "hardware/timer1.h"
#include "logging.h"
#include "sequencer/beat_clock.h"
#include "user_interface/playback_ui.h"
#include "util/bits.h"
#include "util/math.h"
#include "util/usec_timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define DEFAULT_BPM 120
#define QUARTERNOTE_PULSE_LENGTH_US 500
#define MIN_BPM 40
#define MAX_BPM 200

#define MIDI_CLOCK_BYTE 0xF8
#define MIDI_START_BYTE 0xFA
#define MIDI_CONTINUE_BYTE 0xFB
#define MIDI_STOP_BYTE 0xFC

#define USEC_PER_TIMER1_TICK 0.5f

static beat_clock_t g_beat_clock;
static segment_display_t g_segment_display;

/* ----------------------- Interrupt service routines ----------------------- */
ISR(TIMER0_OVF_vect) {
	timer0_timer_overflow_irq();
	static uint8_t last_update = 0;
	last_update++;
	if (last_update > 10) {
		last_update = 0;
		segment_display_update(&g_segment_display); // cycle to next digit
	}
}

ISR(TIMER1_COMPA_vect) {
	beat_clock_on_pulse(&g_beat_clock);
	if (beat_clock_midi_pulse_ready(&g_beat_clock)) {
		sw_serial_write(MIDI_CLOCK_BYTE);
	}
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

// static uint8_t read_midi_byte(void) {
// 	uint8_t midi_byte = 0;
// 	if (sw_serial_available_bytes() > 0) {
// 		sw_serial_read(&midi_byte);
// 	}
// 	return midi_byte;
// }

static void set_playback_tempo(beat_clock_t* beat_clock, uint8_t bpm) {
	beat_clock->tempo_bpm = bpm;
	timer1_set_period((1e6 * 60) / (BEAT_CLOCK_SEQUENCER_PPQN * bpm) / USEC_PER_TIMER1_TICK);
}

int main(void) {
	/* Setup */
	// const gpio_pin_t debug_pin1 = gpio_pin_init_mode(&PORTC, 3, PIN_MODE_OUTPUT);
	// const gpio_pin_t debug_pin2 = gpio_pin_init_mode(&PORTC, 4, PIN_MODE_OUTPUT);
	// const gpio_pin_t pulse_pin = gpio_pin_init_mode(&PORTC, 5, PIN_MODE_OUTPUT);
	const gpio_pin_t midi_rx_pin = gpio_pin_init(&PORTD, 2);
	const gpio_pin_t midi_tx_pin = gpio_pin_init(&PORTD, 3);
	const gpio_pin_t encoder_a_pin = gpio_pin_init(&PORTD, 4);
	const gpio_pin_t encoder_b_pin = gpio_pin_init(&PORTD, 5);
	const gpio_pin_t display_data_pin = gpio_pin_init(&PORTD, 6);
	const gpio_pin_t display_latch_pin = gpio_pin_init(&PORTD, 7);
	const gpio_pin_t display_clock_pin = gpio_pin_init(&PORTB, 0);
	const gpio_pin_t step_buttons_latch_pin = gpio_pin_init(&PORTB, 1);
	// const gpio_pin_t step_leds_latch_pin = gpio_pin_init(&PORTB, 2);

	globally_enable_interrupts();
	// FIXME: refactor timer0 to have similar API as timer1 and make the
	// ms-timer a separate module that gets wired up with timer0 via interrupts
	timer0_initialize();
	hw_serial_initialize(9600); // uses PD0 and PD1
	sw_serial_initialize(31250, midi_rx_pin, midi_tx_pin);
	timer1_initialize();
	spi_t spi = spi_initialize(SPI_DATA_ORDER_MSB_FIRST); // uses PB3, PB4 and PB5
	shift_register_t step_buttons_shift_reg = shift_register_init(spi, step_buttons_latch_pin);
	// shift_register_t step_leds_shift_reg = shift_register_init(spi, step_leds_latch_pin);
	button_t step_buttons[16];
	rotary_encoder_t rotary_encoder = rotary_encoder_init(encoder_a_pin, encoder_b_pin);
	g_segment_display = segment_display_init(display_clock_pin, display_latch_pin, display_data_pin);
	g_beat_clock = beat_clock_init(DEFAULT_BPM);

	playback_ui_t playback_ui = playback_ui_init();

	/* Run */
	// Start beat timer
	{
		set_playback_tempo(&g_beat_clock, DEFAULT_BPM);
		timer1_set_period(10417); // set period to 10417 ticks (96 PPQN => 120 BPM)
		timer1_start();
	}
	LOG_INFO("Program Start\n");
	while (true) {
		/* Input */
		update_button_states(step_buttons, 8, &step_buttons_shift_reg);
		const playback_ui_input_t playback_ui_input = {
			.rotary_encoder_diff = rotary_encoder_read(&rotary_encoder),
		};

		/* Update */
		const playback_ui_events_t playback_events = playback_ui_update(&playback_ui, &playback_ui_input, &g_beat_clock);
		if (playback_events.new_tempo_bpm) {
			set_playback_tempo(&g_beat_clock, playback_events.new_tempo_bpm);
		}
		if (playback_events.start_playback) {
			beat_clock_start(&g_beat_clock);
		}
		if (playback_events.stop_playback) {
			beat_clock_stop(&g_beat_clock);
		}

		/* Output */
		// Tempo display
		segment_display_set_digit(&g_segment_display, 0, playback_ui.display_digits[0]);
		segment_display_set_digit(&g_segment_display, 1, playback_ui.display_digits[1]);
		segment_display_set_digit(&g_segment_display, 2, playback_ui.display_digits[2]);
		segment_display_set_digit(&g_segment_display, 3, playback_ui.display_digits[3]);
	}
}
