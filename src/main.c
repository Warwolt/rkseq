#include "hw_serial.h"
#include "logging.h"
#include "timer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

void globally_enable_interrupts() {
	sei();
}

int main(void) {
	globally_enable_interrupts();
	timer_initialize();
	hw_serial_initialize(9600);
	logging_initialize();

	LOG_INFO("Program Start\n");

	while (true) {
		LOG_INFO("Tick\n");
		_delay_ms(1000);
	}
}
