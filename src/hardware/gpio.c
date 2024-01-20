#include "hardware/gpio.h"

#include "util/bits.h"

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

gpio_pin_t gpio_pin_init(volatile uint8_t* port, uint8_t pin_num) {
	return (gpio_pin_t) {
		.port = port,
		.in_reg = port_to_input_reg(port),
		.num = pin_num
	};
}

gpio_pin_t gpio_pin_init_mode(volatile uint8_t* port, uint8_t pin_num, pin_mode_t mode) {
	gpio_pin_t pin = gpio_pin_init(port, pin_num);
	gpio_pin_configure(pin, mode);
	return pin;
}

void gpio_pin_configure(gpio_pin_t pin, pin_mode_t mode) {
	volatile uint8_t* pin_dir_reg = port_to_pin_direction_reg(pin.port);
	if (mode == PIN_MODE_INPUT) {
		clear_bit(*pin_dir_reg, pin.num);
	}
	if (mode == PIN_MODE_OUTPUT) {
		set_bit(*pin_dir_reg, pin.num);
	}
}

void gpio_pin_write(gpio_pin_t pin, pin_state_t state) {
	if (state == PIN_STATE_LOW) {
		clear_bit(*pin.port, pin.num);
	} else {
		set_bit(*pin.port, pin.num);
	}
}

void gpio_pin_toggle(gpio_pin_t pin) {
	toggle_bit(*pin.port, pin.num);
}

pin_state_t gpio_pin_read(gpio_pin_t pin) {
	return bit_is_set(*pin.in_reg, pin.num) ? PIN_STATE_HIGH : PIN_STATE_LOW;
}
