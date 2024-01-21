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

GpioPin GpioPin_init(volatile uint8_t* port, uint8_t pin_num) {
	return (GpioPin) {
		.port = port,
		.in_reg = port_to_input_reg(port),
		.num = pin_num
	};
}

GpioPin GpioPin_init_mode(volatile uint8_t* port, uint8_t pin_num, PinMode mode) {
	GpioPin pin = GpioPin_init(port, pin_num);
	GpioPin_configure(pin, mode);
	return pin;
}

void GpioPin_configure(GpioPin pin, PinMode mode) {
	volatile uint8_t* pin_dir_reg = port_to_pin_direction_reg(pin.port);
	if (mode == PIN_MODE_INPUT) {
		clear_bit(*pin_dir_reg, pin.num);
	}
	if (mode == PIN_MODE_OUTPUT) {
		set_bit(*pin_dir_reg, pin.num);
	}
}

void GpioPin_write(GpioPin pin, PinState state) {
	if (state == PIN_STATE_LOW) {
		clear_bit(*pin.port, pin.num);
	} else {
		set_bit(*pin.port, pin.num);
	}
}

void GpioPin_toggle(GpioPin pin) {
	toggle_bit(*pin.port, pin.num);
}

PinState GpioPin_read(GpioPin pin) {
	return bit_is_set(*pin.in_reg, pin.num) ? PIN_STATE_HIGH : PIN_STATE_LOW;
}
