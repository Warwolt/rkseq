#include "data/ring_buffer.h"
#include "hardware/button.h"
#include "hardware/gpio.h"
#include "hardware/hw_serial.h"
#include "hardware/rotary_encoder.h"
#include "hardware/segment_display.h"
#include "hardware/sw_serial.h"
#include "hardware/timer0.h"
#include "logging.h"
#include "util/bits.h"
#include "util/math.h"
#include "util/usec_timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define MIN_BPM 40
#define MAX_BPM 200
#define QUARTERNOTE_PULSE_LENGTH_MS 2

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
	const gpio_pin_t led_pin = gpio_pin_init(&PORTB, 5);
	const gpio_pin_t midi_rx_pin = gpio_pin_init(&PORTD, 2);
	const gpio_pin_t midi_tx_pin = gpio_pin_init(&PORTD, 3);
	const gpio_pin_t tempo_knob_a_pin = gpio_pin_init(&PORTD, 4);
	const gpio_pin_t tempo_knob_b_pin = gpio_pin_init(&PORTD, 5);
	const gpio_pin_t display_clock_pin = gpio_pin_init(&PORTD, 6);
	const gpio_pin_t display_latch_pin = gpio_pin_init(&PORTD, 7);
	const gpio_pin_t display_data_pin = gpio_pin_init(&PORTB, 0);

	globally_enable_interrupts();
	timer0_initialize();
	hw_serial_initialize(9600); // uses PD0 and PD1 for logging
	sw_serial_initialize(31250, midi_rx_pin, midi_tx_pin);
	gpio_pin_configure(led_pin, PIN_MODE_OUTPUT);

	const gpio_pin_t start_stop_button_pin = gpio_pin_init(&PORTB, 1);
	gpio_pin_configure(start_stop_button_pin, PIN_MODE_INPUT);
	button_t start_stop_button = button_init();

	rotary_encoder_t tempo_knob = rotary_encoder_init(tempo_knob_a_pin, tempo_knob_b_pin);
	segment_display_t tempo_display = segment_display_init(display_clock_pin, display_latch_pin, display_data_pin);

	/* Run */
	LOG_INFO("Program start!\n");
	uint8_t tempo_bpm = 120;
	usec_timer_t pulse_timer = usec_timer_init(QUARTERNOTE_PULSE_LENGTH_MS * 1e3);
	usec_timer_t quarternote_timer = usec_timer_init((60 * 1e6) / tempo_bpm);
	bool is_playing = false;
	while (true) {
		/* Update buttons */
		button_update(&start_stop_button, gpio_pin_read(start_stop_button_pin));

		/* Update playback */
		bool playback_just_started = false;
		if (button_just_pressed(&start_stop_button)) {
			is_playing = !is_playing;
			playback_just_started = true;
		}

		/* Update Tempo */
		const int rotary_diff = rotary_encoder_read(&tempo_knob);
		tempo_bpm = clamp(tempo_bpm + rotary_diff, MIN_BPM, MAX_BPM);
		quarternote_timer.period_us = (60 * 1e6) / tempo_bpm;

		/* Display Current Tempo*/
		segment_display_set_number(&tempo_display, tempo_bpm * 10);
		segment_display_enable_period(&tempo_display, 1);
		segment_display_update(&tempo_display);

		/* Output tempo pulse */
		if (is_playing) {
			if (usec_timer_period_has_elapsed(&quarternote_timer) || playback_just_started) {
				usec_timer_reset(&quarternote_timer);
				usec_timer_reset(&pulse_timer);
				gpio_pin_set(led_pin);
			}
			if (usec_timer_period_has_elapsed(&pulse_timer)) {
				gpio_pin_clear(led_pin);
			}
		}
	}
}
