#ifndef PLAYBACK_CONTROL_H
#define PLAYBACK_CONTROL_H

#include "sequencer/beat_clock.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t display_digits[4];
} user_interface_t;

typedef struct {
	int8_t rotary_encoder_diff;
} user_interface_input_t;

typedef struct {
	bool start_playback;
	bool stop_playback;
	uint16_t new_tempo_bpm;
} user_interface_events_t;

user_interface_t user_interface_init(void);
user_interface_events_t user_interface_update(user_interface_t* ui, const user_interface_input_t* input, const beat_clock_t* beat_clock);

#endif /* PLAYBACK_CONTROL_H */
