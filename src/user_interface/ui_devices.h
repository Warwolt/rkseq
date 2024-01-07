#ifndef UI_DEVICES_H
#define UI_DEVICES_H

#include "data/button.h"
#include "hardware/rotary_encoder.h"
#include "hardware/segment_display.h"

typedef struct {
	button_t start_button;
	rotary_encoder_t encoder;
	segment_display_t display;
} ui_devices_t;

#endif /* UI_DEVICES_H */
