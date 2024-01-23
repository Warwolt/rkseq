#include "sequencer/step_sequencer.h"

StepSequencer StepSequencer_init(void) {
	return (StepSequencer) {
		.beat_clock = BeatClock_init(DEFAULT_BPM),
	};
}
