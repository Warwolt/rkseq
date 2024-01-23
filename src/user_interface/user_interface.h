#ifndef PLAYBACK_CONTROL_H
#define PLAYBACK_CONTROL_H

#include "sequencer/step_sequencer.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	char segment_display_chars[4];
	bool segment_display_period_enabled[4];
} UserInterface;

typedef struct {
	int8_t rotary_encoder_diff;
} UserInterfaceInput;

typedef struct {
	bool start_playback;
	bool stop_playback;
	uint8_t new_tempo_bpm;
} UserInterfaceEvents;

UserInterface UserInterface_init(void);
UserInterfaceEvents UserInterface_update(UserInterface* ui, const UserInterfaceInput* input, const StepSequencer* step_sequencer);

#endif /* PLAYBACK_CONTROL_H */
