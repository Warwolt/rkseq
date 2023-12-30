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

static volatile uint8_t* port_to_input_reg(volatile uint8_t* port) {
	switch ((intptr_t)port) {
		case (intptr_t)&PORTB:
			return &PINB;
		case (intptr_t)&PORTC:
			return &PINC;
		case (intptr_t)&PORTD:
			return &PIND;
	}
	return NULL;
}

void gpio_pin_configure(gpio_pin_t pin, pin_mode_t mode) {
	volatile uint8_t* pin_dir_reg = port_to_pin_direction_reg(pin.port);
	if (pin_dir_reg) {
		if (mode == PIN_MODE_INPUT) {
			clear_bit(*pin_dir_reg, pin.num);
		}
		if (mode == PIN_MODE_OUTPUT) {
			set_bit(*pin_dir_reg, pin.num);
		}
	}
}

void gpio_pin_write(gpio_pin_t pin, pin_state_t state) {
	if (state == PIN_STATE_LOW) {
		clear_bit(*pin.port, pin.num);
	} else {
		set_bit(*pin.port, pin.num);
	}
}

pin_state_t gpio_pin_read(gpio_pin_t pin) {
	volatile uint8_t* input_reg = port_to_input_reg(pin.port);
	if (!input_reg) {
		return PIN_STATE_LOW;
	}
	return bit_is_set(*input_reg, pin.num);
}
