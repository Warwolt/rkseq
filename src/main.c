#include "bits.h"
#include "gpio.h"
#include "hw_serial.h"
#include "logging.h"
#include "timer0.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define ONBOARD_LED \
	(pin_t) { .port = &PORTB, .num = 5 }

void globally_enable_interrupts(void) {
	sei();
}

ISR(PCINT2_vect) {
	static pin_state_t pin_state = 0;
	pin_write(ONBOARD_LED, pin_state);
	pin_state = !pin_state;
}

int main(void) {
	globally_enable_interrupts();
	timer0_initialize();
	// hw_serial_initialize(9600);
	// logging_initialize();

	pin_configure(ONBOARD_LED, PIN_MODE_OUTPUT);
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

	// 	pin_write(ONBOARD_LED, pin_state);
	// 	pin_state = !pin_state;
	// }
}
