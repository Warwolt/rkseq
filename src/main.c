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

#define ONBOARD_LED \
	(pin_t) { .port = &PORTB, .num = 5 }

void globally_enable_interrupts(void) {
	sei();
}

int main(void) {
	globally_enable_interrupts();
	timer0_initialize();
	serial_t hw_serial = hw_serial_initialize(9600);
	logging_initialize(hw_serial);

	pin_configure(ONBOARD_LED, PIN_MODE_OUTPUT);
	LOG_INFO("Program Start\n");

	bool pin_state = false;
	uint32_t last_tick = timer0_now_ms();
	while (true) {
		uint32_t now = timer0_now_ms();

		if (now - last_tick >= 1000) {
			last_tick = now;
			LOG_INFO("Tick\n");
		}

		pin_write(ONBOARD_LED, pin_state);
		pin_state = !pin_state;
	}
}
