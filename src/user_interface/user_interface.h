#ifndef PLAYBACK_CONTROL_H
#define PLAYBACK_CONTROL_H

#include "sequencer/beat_clock.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	char segment_display_chars[4];
} UserInterface;

typedef struct {
	int8_t rotary_encoder_diff;
} UserInterfaceInput;

typedef struct {
	bool start_playback;
	bool stop_playback;
	uint16_t new_tempo_bpm;
} UserInterfaceEvents;

UserInterface UserInterface_init(void);
UserInterfaceEvents UserInterface_update(UserInterface* ui, const UserInterfaceInput* input, const BeatClock* beat_clock);

#endif /* PLAYBACK_CONTROL_H */
