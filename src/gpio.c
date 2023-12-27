#include "gpio.h"

#include "bits.h"

#include <avr/io.h>
#include <stddef.h>

static volatile uint8_t* port_to_pin_direction_reg(volatile uint8_t* port) {
	switch ((intptr_t)port) {
		case (intptr_t)&PORTB:
			return &DDRB;
		case (intptr_t)&PORTC:
			return &DDRC;
		case (intptr_t)&PORTD:
			return &DDRD;
	}
	return NULL;
}

void pin_configure(pin_t pin, pin_mode_t mode) {
	volatile uint8_t* pin_dir_reg = port_to_pin_direction_reg(pin.port);
	if (!pin_dir_reg) {
		return;
	}
	if (mode == PIN_MODE_INPUT) {
		clear_bit(*pin_dir_reg, pin.num);
	}
	if (mode == PIN_MODE_OUTPUT) {
		set_bit(*pin_dir_reg, pin.num);
	}
}

void pin_write(pin_t pin, pin_state_t state) {
	if (state == PIN_STATE_CLEAR) {
		clear_bit(*pin.port, pin.num);
	}
	if (state == PIN_STATE_SET) {
		set_bit(*pin.port, pin.num);
	}
}

pin_state_t pin_read(pin_t pin) {
	return bit_is_set(pin.port, pin.num);
}
