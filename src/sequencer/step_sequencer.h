#ifndef STEP_SEQUENCER_H
#define STEP_SEQUENCER_H

#include "sequencer/beat_clock.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define STEP_SEQUENCER_NUM_STEP_PATTERNS 1

typedef struct {
	bool playback_is_active;
	size_t current_step;
	bool step_patterns[STEP_SEQUENCER_NUM_STEP_PATTERNS][16];
	BeatClock beat_clock;
} StepSequencer;

StepSequencer StepSequencer_init(void);
bool StepSequencer_current_step_in_pattern_is_active(const StepSequencer* step_sequencer, uint8_t pattern);
void StepSequencer_increment_step(StepSequencer* step_sequencer);

#endif /* STEP_SEQUENCER_H */
