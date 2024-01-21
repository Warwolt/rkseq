#ifndef PLAYBACK_CONTROL_H
#define PLAYBACK_CONTROL_H

#include "sequencer/beat_clock.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t display_digits[4];
} playback_ui_t;

typedef struct {
	int8_t rotary_encoder_diff;
} playback_ui_input_t;

typedef struct {
	bool start_playback;
	bool stop_playback;
	uint16_t new_tempo_bpm;
} playback_ui_events_t;

playback_ui_t playback_ui_init(void);
playback_ui_events_t playback_ui_update(playback_ui_t* playback_ui, const playback_ui_input_t* input, const beat_clock_t* beat_clock);

#endif /* PLAYBACK_CONTROL_H */
