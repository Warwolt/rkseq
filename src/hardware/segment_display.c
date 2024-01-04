#include "hardware/segment_display.h"

static int8_t digit_segments[10] = {
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01100111, // 9
};

segment_display_t segment_display_init(gpio_pin_t clock_pin, gpio_pin_t latch_pin, gpio_pin_t data_pin) {
	gpio_pin_configure(clock_pin, PIN_MODE_OUTPUT);
	gpio_pin_configure(latch_pin, PIN_MODE_OUTPUT);
	gpio_pin_configure(data_pin, PIN_MODE_OUTPUT);
	return (segment_display_t) {
		.clock_pin = clock_pin,
		.latch_pin = latch_pin,
		.data_pin = data_pin,
		.digits = { 0 },
		.current_digit = 0,
	};
}

void segment_display_enable_period(segment_display_t* display, uint8_t number) {
	if (number < 4) {
		display->period[number] = 1;
	}
}

void segment_display_disable_period(segment_display_t* display, uint8_t number) {
	if (number < 4) {
		display->period[number] = 0;
	}
}

void segment_display_set_number(segment_display_t* display, uint16_t number) {
	display->digits[0] = number / 1 % 10;
	display->digits[1] = number / 10 % 10;
	display->digits[2] = number / 100 % 10;
	display->digits[3] = number / 1000 % 10;
}

void segment_display_update(segment_display_t* display) {
	// write segments
	int8_t byte = 0;
	if (display->current_digit < 4) {
		byte = ~digit_segments[display->digits[display->current_digit]];
		byte &= ~(display->period[display->current_digit] << (8 - 1));
	}
	for (uint8_t i = 0; i < 8; i++) {
		const uint8_t bit = (byte >> ((8 - 1) - i)) & 1;
		gpio_pin_write(display->data_pin, bit);
		gpio_pin_set(display->clock_pin);
		gpio_pin_clear(display->clock_pin);
	}
	// select digit
	for (uint8_t i = 0; i < 8; i++) {
		const uint8_t bit = ((0x1 << display->current_digit) >> ((8 - 1) - i)) & 1;
		gpio_pin_write(display->data_pin, bit);
		gpio_pin_set(display->clock_pin);
		gpio_pin_clear(display->clock_pin);
	}
	// output digit
	gpio_pin_set(display->latch_pin);
	gpio_pin_clear(display->latch_pin);

	display->current_digit = (display->current_digit + 1) % 16;
}
