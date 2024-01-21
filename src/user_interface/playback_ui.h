#ifndef PLAYBACK_CONTROL_H
#define PLAYBACK_CONTROL_H

#include "sequencer/beat_clock.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t segment_display[4];
} playback_ui_t;

typedef struct {
	int8_t rotary_encoder_diff;
	bool start_button_pressed_now;
} playback_ui_input_t;

typedef struct {
	bool start_playback;
	bool stop_playback;
	int8_t tempo_diff;
} playback_ui_events_t;

playback_ui_t playback_ui_init(void);
playback_ui_events_t playback_ui_update(playback_ui_t* playback_ui, const playback_ui_input_t* input, const beat_clock_t* beat_clock);

#endif /* PLAYBACK_CONTROL_H */
