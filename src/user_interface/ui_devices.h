#ifndef UI_DEVICES_H
#define UI_DEVICES_H

#include "data/button.h"
#include "hardware/rotary_encoder.h"
#include "hardware/segment_display.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	int8_t rotary_diff;
	bool start_button_pressed_now;
} ui_devices_input_t;

#endif /* UI_DEVICES_H */
