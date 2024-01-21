#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include "hardware/gpio.h"

#include <stdint.h>

typedef struct {
	GpioPin pin_a;
	GpioPin pin_b;
	PinState prev_a;
} rotary_encoder_t;

rotary_encoder_t rotary_encoder_init(GpioPin pin_a, GpioPin pin_b);
int8_t rotary_encoder_read(rotary_encoder_t* encoder);

#endif /* ROTARY_ENCODER_H */
