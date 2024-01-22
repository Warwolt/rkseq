#include "hardware/segment_display.h"

static int8_t character_segments[128] = {
	[' '] = 0b00000000,
	['-'] = 0b01000000,
	['0'] = 0b00111111,
	['1'] = 0b00000110,
	['2'] = 0b01011011,
	['3'] = 0b01001111,
	['4'] = 0b01100110,
	['5'] = 0b01101101,
	['6'] = 0b01111101,
	['7'] = 0b00000111,
	['8'] = 0b01111111,
	['9'] = 0b01100111,
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
		.characters = { 0 },
		.current_character = 0,
	};
}

void SegmentDisplay_output_next_char(SegmentDisplay* display) {
	// write segments
	int8_t byte = 0;
	if (display->current_character < 4) {
		const uint8_t digit = display->characters[display->current_character];
		const uint8_t segments = character_segments[digit];
		const uint8_t period = display->period_enabled[display->current_character] << (8 - 1);
		byte = ~(segments | period);
	}
	SegmentDisplay_output_byte(display, byte);
	SegmentDisplay_output_byte(display, 0x1 << display->current_character);

	// output character
	GpioPin_set(display->latch_pin);
	GpioPin_clear(display->latch_pin);

	// cycle next index, index > 7 outputs no digit which dims the display
	// since the characters now have a duty cycle less than 100 %
	display->current_character = (display->current_character + 1) % 5;
}

void SegmentDisplay_set_char(SegmentDisplay* display, uint8_t position, char character) {
	display->characters[position] = character;
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
