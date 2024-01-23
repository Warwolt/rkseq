#ifndef STEP_SEQUENCER_H
#define STEP_SEQUENCER_H

#include "sequencer/beat_clock.h"

typedef struct {
	BeatClock beat_clock;
} StepSequencer;

StepSequencer StepSequencer_init(void);

#endif /* STEP_SEQUENCER_H */
