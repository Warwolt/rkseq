#ifndef SEGMENT_DISPLAY_H
#define SEGMENT_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#include "hardware/gpio.h"

typedef struct {
	gpio_pin_t clock_pin;
	gpio_pin_t latch_pin;
	gpio_pin_t data_pin;
	bool period[4];
	uint8_t digits[4];
	uint8_t current_digit;
} segment_display_t;

segment_display_t segment_display_init(gpio_pin_t clock_pin, gpio_pin_t latch_pin, gpio_pin_t data_pin);
void segment_display_enable_period(segment_display_t* display, uint8_t number);
void segment_display_disable_period(segment_display_t* display, uint8_t number);
void segment_display_set_number(segment_display_t* display, uint16_t number);
void segment_display_update(segment_display_t* display);

#endif /* SEGMENT_DISPLAY_H */
