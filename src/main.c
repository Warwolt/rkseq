#include "bits.h"
#include "gpio.h"
#include "hw_serial.h"
#include "logging.h"
#include "sw_serial.h"
#include "timer0.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define LED_PIN \
	(gpio_pin_t) { .port = &PORTB, .num = 5 }

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

/* ------------------------------ Program Main ------------------------------ */

void globally_enable_interrupts(void) {
	sei();
}

int main(void) {
	globally_enable_interrupts();
	timer0_initialize();
	hw_serial_initialize(9600);
	logging_initialize();

	gpio_pin_configure(LED_PIN, PIN_MODE_OUTPUT);
	LOG_INFO("Program Start\n");

	// Setup pin change interrupts
	// {
	// 	set_bit(PCICR, PCIE2); // enable pin change interrupts
	// 	set_bit(PCMSK2, PCINT16); // configure PD0-pin (Rx) to trigger interrupts
	// }

	// while (true) {
	// }

	uint32_t last_tick = timer0_now_ms();
	while (true) {
		uint32_t now = timer0_now_ms();

		if (now - last_tick >= 1000) {
			last_tick = now;
			gpio_pin_set(LED_PIN);
			LOG_INFO("Tick\n");
			gpio_pin_clear(LED_PIN);
		}
	}
}
