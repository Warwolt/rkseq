#include "hardware/rotary_encoder.h"

rotary_encoder_t rotary_encoder_init(gpio_pin_t pin_a, gpio_pin_t pin_b) {
	gpio_pin_configure(pin_a, PIN_MODE_INPUT);
	gpio_pin_configure(pin_b, PIN_MODE_INPUT);
	return (rotary_encoder_t) {
		.pin_a = pin_a,
		.pin_b = pin_b,
		.prev_a = gpio_pin_read(pin_a)
	};
}

int8_t rotary_encoder_read(rotary_encoder_t* encoder) {
	int8_t rotary_diff = 0;
	pin_state_t current_a = gpio_pin_read(encoder->pin_a);
	if (encoder->prev_a == 1 && current_a == 0) {
		if (gpio_pin_read(encoder->pin_b) == 0) {
			rotary_diff = 1; // clockwise
		} else {
			rotary_diff = -1; // counter clockwise
		}
	}
	encoder->prev_a = current_a;
	return rotary_diff;
}
