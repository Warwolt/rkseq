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

/* ----------------------- Interrupt service routines ----------------------- */
ISR(TIMER0_OVF_vect) {
	timer0_timer_overflow_irq();
}

#define BAUD 9600
#define BIT_PERIOD_NS (1e9 / BAUD)
#define NS_PER_4_INSTRUCTIONS (4 * 1e9 / F_CPU)
#define BIT_PERIOD_DELAY (BIT_PERIOD_NS / NS_PER_4_INSTRUCTIONS) // delay in units of 4 instructions

static ringbuffer_t g_rx_buffer = { 0 };

ISR(PCINT2_vect) {
	if (gpio_pin_read(RX_PIN) == 0) {
		uint8_t byte = 0;

		_delay_loop_2(BIT_PERIOD_DELAY * 1.5);
		for (int i = 0; i < 8; i++) {
			const uint8_t bit = gpio_pin_read(RX_PIN);
			byte |= bit << i;

			// debug begin
			gpio_pin_toggle(LED_PIN);
			gpio_pin_toggle(LED_PIN);
			_delay_loop_2(BIT_PERIOD_DELAY * 0.90);
			// debug end

			// _delay_loop_2(BIT_PERIOD_DELAY);
		}

		ringbuffer_write(&g_rx_buffer, byte);
	}
}

ISR(USART_RX_vect) {
	hw_serial_rx_complete_irq();
}

ISR(USART_UDRE_vect) {
	hw_serial_tx_udr_empty_irq();
}

// TODO: add an interrupt for timer1/timer2 here and connect it to the software UART code

/* ------------------------------ Main Program ------------------------------ */
void globally_enable_interrupts(void) {
	sei();
}

int main(void) {
	globally_enable_interrupts();
	// timer0_initialize();
	// hw_serial_initialize(9600);
	gpio_pin_configure(LED_PIN, PIN_MODE_OUTPUT);
	gpio_pin_configure(RX_PIN, PIN_MODE_INPUT);

	// Enable interrupts on RX_PIN
	{
		set_bit(PCICR, PCIE2); // enable pin change interrupts
		set_bit(PCMSK2, PCINT16); // configure PD0-pin (Rx) to trigger interrupts
	}

	while (true) {
		if (!ringbuffer_is_empty(&g_rx_buffer)) {
			uint8_t byte;
			ringbuffer_read(&g_rx_buffer, &byte);

			_delay_us(100);

			// byte
			for (int i = 0; i < 8; i++) {
				const uint8_t bit = (byte >> (8 - 1 - i)) & 0x1;
				gpio_pin_toggle(LED_PIN);
				gpio_pin_toggle(LED_PIN);
				gpio_pin_write(LED_PIN, bit);
				_delay_loop_2(BIT_PERIOD_DELAY * 0.90);
			}
			gpio_pin_toggle(LED_PIN);
			gpio_pin_toggle(LED_PIN);
		}
		gpio_pin_write(LED_PIN, 0);
	}

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
