#include "data/ring_buffer.h"
#include "hardware/gpio.h"
#include "hardware/hw_serial.h"
#include "hardware/rotary_encoder.h"
#include "hardware/sw_serial.h"
#include "hardware/timer0.h"
#include "logging.h"
#include "util/bits.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

const gpio_pin_t LED_PIN = { .port = &PORTB, .num = 5 };
const gpio_pin_t MIDI_RX_PIN = { .port = &PORTD, .num = 2 };
const gpio_pin_t MIDI_TX_PIN = { .port = &PORTD, .num = 3 };
const gpio_pin_t TEMPO_KNOB_A_PIN = { .port = &PORTD, .num = 4 };
const gpio_pin_t TEMPO_KNOB_B_PIN = { .port = &PORTD, .num = 5 };

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
	globally_enable_interrupts();
	timer0_initialize();
	hw_serial_initialize(9600); // uses PD0 and PD1 for logging
	sw_serial_initialize(31250, MIDI_RX_PIN, MIDI_TX_PIN);
	gpio_pin_configure(LED_PIN, PIN_MODE_OUTPUT);
	rotary_encoder_t tempo_knob = rotary_encoder_init(TEMPO_KNOB_A_PIN, TEMPO_KNOB_B_PIN);

	LOG_INFO("Program Start\n");
	uint32_t last_tick = timer0_now_ms();
	while (true) {
		uint32_t now = timer0_now_ms();
		if (now - last_tick >= 1000) {
			last_tick = now;
			gpio_pin_set(LED_PIN);
			LOG_INFO("Tick\n");
			gpio_pin_clear(LED_PIN);
		}

		// update rotary encoder
		int rotary_diff = rotary_encoder_read(&tempo_knob);
		if (rotary_diff != 0) {
			LOG_INFO("%d\n", rotary_diff);
		}
	}
}
