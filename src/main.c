#include "bits.h"
#include "gpio.h"
#include "hw_serial.h"
#include "logging.h"
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
	static pin_state_t pin_state = 0;
	pin_state = !pin_state;
	gpio_pin_write(LED_PIN, pin_state);

	// TODO: Trigger timer1 interrupts here at 2 * 9600 Khz on start bit
	//
	// The timer should then later be used to sample the state of the Rx pin
	// in the middle of the baud rate clock period and use this for software uart.
	//
	// ___     ____      ________
	//   |    |    |    |   :   |
	//   |____|    |____|   :   |____
	//   ^      ^    ^    ^   ^
	//   |      |    |    |   |
	// start    |  data1  | data3
	//        data0     data2
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
	// hw_serial_initialize(9600);
	// logging_initialize();

	gpio_pin_configure(LED_PIN, PIN_MODE_OUTPUT);
	LOG_INFO("Program Start\n");

	// Setup pin change interrupts
	{
		set_bit(PCICR, PCIE2); // enable pin change interrupts
		set_bit(PCMSK2, PCINT16); // configure PD0-pin (Rx) to trigger interrupts
	}

	while (true) {
	}

	// bool pin_state = false;
	// uint32_t last_tick = timer0_now_ms();
	// while (true) {
	// 	uint32_t now = timer0_now_ms();

	// 	if (now - last_tick >= 1000) {
	// 		last_tick = now;
	// 		LOG_INFO("Tick\n");
	// 	}

	// 	gpio_pin_write(LED_PIN, pin_state);
	// 	pin_state = !pin_state;
	// }
}
