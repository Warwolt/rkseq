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

static void segment_display_output_byte(segment_display_t* display, uint8_t byte) {
	for (uint8_t i = 0; i < 8; i++) {
		const uint8_t bit = (byte >> ((8 - 1) - i)) & 1;
		gpio_pin_write(display->data_pin, bit);
		gpio_pin_set(display->clock_pin);
		gpio_pin_clear(display->clock_pin);
	}
}

segment_display_t segment_display_init(gpio_pin_t clock_pin, gpio_pin_t latch_pin, gpio_pin_t data_pin) {
	gpio_pin_configure(clock_pin, PIN_MODE_OUTPUT);
	gpio_pin_configure(latch_pin, PIN_MODE_OUTPUT);
	gpio_pin_configure(data_pin, PIN_MODE_OUTPUT);
	return (segment_display_t) {
		.clock_pin = clock_pin,
		.latch_pin = latch_pin,
		.data_pin = data_pin,
		.digits = { 0 },
		.digits_index = 0,
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
	int8_t segments = 0;
	if (display->digits_index < 4) {
		segments = ~digit_segments[display->digits[display->digits_index]];
		segments &= ~(display->period[display->digits_index] << (8 - 1));
	}
	segment_display_output_byte(display, segments);
	segment_display_output_byte(display, 0x1 << display->digits_index);

	// output digit
	gpio_pin_set(display->latch_pin);
	gpio_pin_clear(display->latch_pin);

	// cycle next index, index > 7 outputs no digit which dims the display
	// since the digits now have a duty cycle less than 100 %
	display->digits_index = (display->digits_index + 1) % 16;
}
