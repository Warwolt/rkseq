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
	uint8_t characters[4];
	uint8_t current_character;
} SegmentDisplay;

SegmentDisplay SegmentDisplay_init(GpioPin clock_pin, GpioPin latch_pin, GpioPin data_pin);
void SegmentDisplay_update(SegmentDisplay* display);
void SegmentDisplay_set_char(SegmentDisplay* display, uint8_t position, char character);
void SegmentDisplay_enable_period(SegmentDisplay* display, uint8_t number);
void SegmentDisplay_disable_period(SegmentDisplay* display, uint8_t number);

#endif /* SEGMENT_DISPLAY_H */
