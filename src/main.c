#include "bits.h"
#include "gpio.h"
#include "hw_serial.h"
#include "logging.h"
#include "serial.h"
#include "timer0.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define LED_PIN \
	(gpio_pin_t) { .port = &PORTB, .num = 5 }

/* ----------------------- Interrupt service routines ----------------------- */
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
	hw_serial_initialize(9600);

	gpio_pin_configure(LED_PIN, PIN_MODE_OUTPUT);
	LOG_INFO("Program Start\n");

	pin_state_t pin_state = 0;
	uint32_t last_tick = timer0_now_ms();
	while (true) {
		uint32_t now = timer0_now_ms();

		if (now - last_tick >= 1000) {
			last_tick = now;
			LOG_INFO("Tick\n");
		}

		gpio_pin_write(LED_PIN, pin_state);
		pin_state = !pin_state;
	}
}
