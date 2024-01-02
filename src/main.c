#include "bits.h"
#include "gpio.h"
#include "hw_serial.h"
#include "logging.h"
#include "ringbuffer.h"
#include "sw_serial.h"
#include "timer0.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define LED_PIN \
	(gpio_pin_t) { .port = &PORTB, .num = 5 }

#define RX_PIN \
	(gpio_pin_t) { .port = &PORTD, .num = 0 }

#define TX_PIN \
	(gpio_pin_t) { .port = &PORTD, .num = 1 }

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
	// timer0_initialize();
	// hw_serial_initialize(9600);
	sw_serial_initialize(31250, RX_PIN, TX_PIN);
	gpio_pin_configure(LED_PIN, PIN_MODE_OUTPUT);

	while (true) {
		char* str = "Hello MIDI baud!";
		while (*str) {
			sw_serial_write(*str);
			str++;
		}
		_delay_ms(1000);
		// echo back any input
		// if (sw_serial_available_bytes() > 0) {
		// 	uint8_t byte;
		// 	sw_serial_read(&byte);
		// 	sw_serial_write(byte);
		// }
	}

	// while (true) {
	// 	if (sw_serial_available_bytes() > 0) {
	// 		uint8_t byte;
	// 		sw_serial_read_bytes(&byte, 1);

	// 		_delay_us(100);

	// 		// byte
	// 		for (int i = 0; i < 8; i++) {
	// 			const uint8_t bit = (byte >> (8 - 1 - i)) & 0x1;
	// 			gpio_pin_toggle(LED_PIN);
	// 			gpio_pin_toggle(LED_PIN);
	// 			gpio_pin_write(LED_PIN, bit);
	// 			_delay_us(32);
	// 		}
	// 		gpio_pin_toggle(LED_PIN);
	// 		gpio_pin_toggle(LED_PIN);
	// 	}
	// 	gpio_pin_write(LED_PIN, 0);
	// }

	// LOG_INFO("Program Start\n");
	// uint32_t last_tick = timer0_now_ms();
	// while (true) {
	// 	uint32_t now = timer0_now_ms();

	// 	if (now - last_tick >= 1000) {
	// 		last_tick = now;
	// 		gpio_pin_set(LED_PIN);
	// 		LOG_INFO("Tick\n");
	// 		gpio_pin_clear(LED_PIN);
	// 	}
	// }
}
