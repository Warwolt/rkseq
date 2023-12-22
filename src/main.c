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
	(pin_t) { .port = PORT_B, .num = 5 }

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

	while (true) {
		pin_set(ONBOARD_LED);
		LOG_INFO("Tick\n");
		_delay_ms(1000);

		pin_clear(ONBOARD_LED);
		LOG_INFO("Tick\n");
		_delay_ms(1000);
	}
}
