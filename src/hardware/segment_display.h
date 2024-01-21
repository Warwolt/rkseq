#ifndef SEGMENT_DISPLAY_H
#define SEGMENT_DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

#include "hardware/gpio.h"

typedef struct {
	GpioPin clock_pin;
	GpioPin latch_pin;
	GpioPin data_pin;
	bool period_enabled[4];
	uint8_t digits[4];
	uint8_t digits_index;
} SegmentDisplay;

SegmentDisplay SegmentDisplay_init(GpioPin clock_pin, GpioPin latch_pin, GpioPin data_pin);
void SegmentDisplay_enable_period(SegmentDisplay* display, uint8_t number);
void SegmentDisplay_disable_period(SegmentDisplay* display, uint8_t number);
void SegmentDisplay_set_number(SegmentDisplay* display, uint16_t number);
void SegmentDisplay_set_digit(SegmentDisplay* display, uint16_t digit, uint8_t value);
void SegmentDisplay_update(SegmentDisplay* display);

#endif /* SEGMENT_DISPLAY_H */
