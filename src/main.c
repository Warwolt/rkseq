#include "bits.h"
#include "hw_serial.h"
#include "logging.h"
#include "serial.h"
#include "timer0.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

typedef enum {
	PORT_B,
	PORT_C,
	PORT_D
} port_t;

typedef struct {
	port_t port;
	int num;
} pin_t;

typedef enum {
	PIN_MODE_INPUT,
	PIN_MODE_OUTPUT,
} pin_mode_t;

#define ONBOARD_LED \
	(pin_t) { .port = PORT_B, .num = 5 }

void globally_enable_interrupts(void) {
	sei();
}

void pin_configure(pin_t pin, pin_mode_t mode) {
	if (mode == PIN_MODE_INPUT) {
		switch (pin.port) {
			case PORT_B:
				clear_bit(DDRB, pin.num);
				break;
			case PORT_C:
				clear_bit(DDRC, pin.num);
				break;
			case PORT_D:
				clear_bit(DDRD, pin.num);
				break;
		}
	}

	if (mode == PIN_MODE_OUTPUT) {
		switch (pin.port) {
			case PORT_B:
				set_bit(DDRB, pin.num);
				break;
			case PORT_C:
				set_bit(DDRC, pin.num);
				break;
			case PORT_D:
				set_bit(DDRD, pin.num);
				break;
		}
	}
}

void pin_set(pin_t pin) {
	switch (pin.port) {
		case PORT_B:
			set_bit(PORTB, pin.num);
			break;

		case PORT_C:
			set_bit(PORTC, pin.num);
			break;

		case PORT_D:
			set_bit(PORTD, pin.num);
			break;
	}
}

void pin_clear(pin_t pin) {
	switch (pin.port) {
		case PORT_B:
			clear_bit(PORTB, pin.num);
			break;

		case PORT_C:
			clear_bit(PORTC, pin.num);
			break;

		case PORT_D:
			clear_bit(PORTD, pin.num);
			break;
	}
}

int main(void) {
	globally_enable_interrupts();
	timer0_initialize();
	serial_t serial = hw_serial_initialize(9600);
	logging_initialize(serial);

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
