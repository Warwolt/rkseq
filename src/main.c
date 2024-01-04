#include "data/ring_buffer.h"
#include "hardware/gpio.h"
#include "hardware/hw_serial.h"
#include "hardware/rotary_encoder.h"
#include "hardware/segment_display.h"
#include "hardware/sw_serial.h"
#include "hardware/timer0.h"
#include "logging.h"
#include "util/bits.h"
#include "util/math.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define MIN_BPM 40
#define MAX_BPM 200

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

	rotary_encoder_t tempo_knob = rotary_encoder_init(tempo_knob_a_pin, tempo_knob_b_pin);
	segment_display_t tempo_display = segment_display_init(display_clock_pin, display_latch_pin, display_data_pin);

	LOG_INFO("Program Start\n");
	uint64_t last_bpm_tick_us = timer0_now_us();
	uint64_t last_pulse_tick_us = timer0_now_us();
	uint8_t tempo_bpm = 120;
	uint64_t quarternote_period_us = (60 * 1e6) / tempo_bpm;
	while (true) {
		const uint64_t now_us = timer0_now_us();

		// update tempo
		int rotary_diff = rotary_encoder_read(&tempo_knob);
		tempo_bpm = clamp(tempo_bpm + rotary_diff, MIN_BPM, MAX_BPM);
		quarternote_period_us = (60 * 1e6) / tempo_bpm;

		// display tempo
		segment_display_set_number(&tempo_display, tempo_bpm * 10);
		segment_display_enable_period(&tempo_display, 1);
		segment_display_update(&tempo_display);

		// output tick
		if (now_us - last_bpm_tick_us >= quarternote_period_us) {
			last_bpm_tick_us = now_us;
			last_pulse_tick_us = now_us;
			gpio_pin_set(led_pin);
		}

		if (now_us - last_pulse_tick_us >= 20000) {
			gpio_pin_clear(led_pin);
		}
	}
}
