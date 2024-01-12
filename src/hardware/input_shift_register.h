#ifndef INPUT_SHIFT_REGISTER_H
#define INPUT_SHIFT_REGISTER_H

#include "hardware/gpio.h"

#include <stdint.h>

// 74HC165 input shift register
typedef struct {
	gpio_pin_t latch_pin;
} input_shift_register_t;

input_shift_register_t input_shift_register_init(gpio_pin_t latch_pin);
uint16_t input_shift_register_read(const input_shift_register_t* shift_reg);

#endif /* INPUT_SHIFT_REGISTER_H */
