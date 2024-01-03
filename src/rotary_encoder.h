#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include "gpio.h"

#include <stdint.h>

typedef struct {
	gpio_pin_t pin_a;
	gpio_pin_t pin_b;
	pin_state_t prev_a;
} rotary_encoder_t;

rotary_encoder_t rotary_encoder_init(gpio_pin_t pin_a, gpio_pin_t pin_b);
int8_t rotary_encoder_read(rotary_encoder_t* encoder);

#endif /* ROTARY_ENCODER_H */
