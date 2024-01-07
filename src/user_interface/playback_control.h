#ifndef PLAYBACK_CONTROL_H
#define PLAYBACK_CONTROL_H

#include "data/button.h"
#include "hardware/rotary_encoder.h"
#include "hardware/segment_display.h"
#include "sequencer/beat_clock.h"

typedef struct {
	button_t start_button;
	rotary_encoder_t tempo_knob;
	segment_display_t tempo_display;
} ui_devices_t;

void playback_control_update(ui_devices_t* ui_devices, beat_clock_t* beat_clock);

#endif /* PLAYBACK_CONTROL_H */
