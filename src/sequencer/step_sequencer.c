#include "sequencer/step_sequencer.h"

StepSequencer StepSequencer_init(void) {
	return (StepSequencer) {
		.step_index = 0,
		// clang-format off
		.step_patterns = {{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
		// clang-format on
		.beat_clock = BeatClock_init(DEFAULT_TEMPO),
	};
}
