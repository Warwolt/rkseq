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

	globally_enable_interrupts();
	timer0_initialize();
	hw_serial_initialize(9600); // uses PD0 and PD1 for logging
	sw_serial_initialize(31250, midi_rx_pin, midi_tx_pin);

	gpio_pin_configure(led_pin, PIN_MODE_OUTPUT);
	rotary_encoder_t tempo_knob = rotary_encoder_init(tempo_knob_a_pin, tempo_knob_b_pin);

	LOG_INFO("Program Start\n");
	uint32_t last_tick = timer0_now_ms();
	while (true) {
		uint32_t now = timer0_now_ms();
		if (now - last_tick >= 1000) {
			last_tick = now;
			gpio_pin_set(led_pin);
			LOG_INFO("Tick\n");
			gpio_pin_clear(led_pin);
		}

		int rotary_diff = rotary_encoder_read(&tempo_knob);
		if (rotary_diff != 0) {
			LOG_INFO("%d\n", rotary_diff);
		}
	}
}
