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
} segment_display_t;

segment_display_t segment_display_init(GpioPin clock_pin, GpioPin latch_pin, GpioPin data_pin);
void segment_display_enable_period(segment_display_t* display, uint8_t number);
void segment_display_disable_period(segment_display_t* display, uint8_t number);
void segment_display_set_number(segment_display_t* display, uint16_t number);
void segment_display_set_digit(segment_display_t* display, uint16_t digit, uint8_t value);
void segment_display_update(segment_display_t* display);

#endif /* SEGMENT_DISPLAY_H */
