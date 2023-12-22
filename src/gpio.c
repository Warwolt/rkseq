#include "gpio.h"

#include "bits.h"

#include <avr/io.h>

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
