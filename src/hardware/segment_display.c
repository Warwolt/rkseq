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

static void SegmentDisplay_output_byte(SegmentDisplay* display, uint8_t byte) {
	for (uint8_t i = 0; i < 8; i++) {
		const uint8_t bit = (byte >> ((8 - 1) - i)) & 1;
		GpioPin_write(display->data_pin, bit);
		GpioPin_set(display->clock_pin);
		GpioPin_clear(display->clock_pin);
	}
}

SegmentDisplay SegmentDisplay_init(GpioPin clock_pin, GpioPin latch_pin, GpioPin data_pin) {
	GpioPin_configure(clock_pin, PIN_MODE_OUTPUT);
	GpioPin_configure(latch_pin, PIN_MODE_OUTPUT);
	GpioPin_configure(data_pin, PIN_MODE_OUTPUT);
	return (SegmentDisplay) {
		.clock_pin = clock_pin,
		.latch_pin = latch_pin,
		.data_pin = data_pin,
		.digits = { 0 },
		.digits_index = 0,
	};
}

void SegmentDisplay_enable_period(SegmentDisplay* display, uint8_t number) {
	if (number < 4) {
		display->period_enabled[number] = 1;
	}
}

void SegmentDisplay_disable_period(SegmentDisplay* display, uint8_t number) {
	if (number < 4) {
		display->period_enabled[number] = 0;
	}
}

void SegmentDisplay_set_number(SegmentDisplay* display, uint16_t number) {
	display->digits[0] = number / 1 % 10;
	display->digits[1] = number / 10 % 10;
	display->digits[2] = number / 100 % 10;
	display->digits[3] = number / 1000 % 10;
}

void SegmentDisplay_update(SegmentDisplay* display) {
	// write segments
	int8_t byte = 0;
	if (display->digits_index < 4) {
		const uint8_t digit = display->digits[display->digits_index];
		const uint8_t segments = digit_segments[digit];
		const uint8_t period = display->period_enabled[display->digits_index] << (8 - 1);
		byte = ~(segments | period);
	}
	SegmentDisplay_output_byte(display, byte);
	SegmentDisplay_output_byte(display, 0x1 << display->digits_index);

	// output digit
	GpioPin_set(display->latch_pin);
	GpioPin_clear(display->latch_pin);

	// cycle next index, index > 7 outputs no digit which dims the display
	// since the digits now have a duty cycle less than 100 %
	display->digits_index = (display->digits_index + 1) % 12;
}

void SegmentDisplay_set_digit(SegmentDisplay* display, uint16_t digit, uint8_t value) {
	display->digits[digit] = value;
}
