#include "bits.h"
#include "hw_serial.h"
#include "logging.h"
#include "serial.h"
#include "timer0.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#define ONBOARD_LED 5

void globally_enable_interrupts(void) {
	sei();
}

int main(void) {
	globally_enable_interrupts();
	timer0_initialize();
	serial_t serial = hw_serial_initialize(9600);
	logging_initialize(serial);

	set_bit(DDRB, ONBOARD_LED);
	clear_bit(PORTB, ONBOARD_LED);

	LOG_INFO("Program Start\n");

	while (true) {
		LOG_INFO("Tick\n");
		_delay_ms(1000);
	}
}
