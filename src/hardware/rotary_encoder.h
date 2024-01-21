#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include "hardware/gpio.h"

#include <stdint.h>

typedef struct {
	GpioPin pin_a;
	GpioPin pin_b;
	PinState prev_a;
} RotaryEncoder;

RotaryEncoder RotaryEncoder_init(GpioPin pin_a, GpioPin pin_b);
int8_t RotaryEncoder_read(RotaryEncoder* encoder);

#endif /* ROTARY_ENCODER_H */
