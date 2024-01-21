#include "hardware/rotary_encoder.h"

rotary_encoder_t rotary_encoder_init(GpioPin pin_a, GpioPin pin_b) {
	GpioPin_configure(pin_a, PIN_MODE_INPUT);
	GpioPin_configure(pin_b, PIN_MODE_INPUT);
	return (rotary_encoder_t) {
		.pin_a = pin_a,
		.pin_b = pin_b,
		.prev_a = GpioPin_read(pin_a)
	};
}

int8_t rotary_encoder_read(rotary_encoder_t* encoder) {
	int8_t rotary_diff = 0;
	PinState current_a = GpioPin_read(encoder->pin_a);
	if (encoder->prev_a == 1 && current_a == 0) {
		if (GpioPin_read(encoder->pin_b) == 0) {
			rotary_diff = 1; // clockwise
		} else {
			rotary_diff = -1; // counter clockwise
		}
	}
	encoder->prev_a = current_a;
	return rotary_diff;
}
