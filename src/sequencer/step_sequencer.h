#ifndef STEP_SEQUENCER_H
#define STEP_SEQUENCER_H

#include "sequencer/beat_clock.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct {
	bool playback_is_active;
	size_t step_index;
	bool step_pattern[16];
	BeatClock beat_clock;
} StepSequencer;

StepSequencer StepSequencer_init(void);

#endif /* STEP_SEQUENCER_H */
