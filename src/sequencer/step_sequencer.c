#include "sequencer/step_sequencer.h"

StepSequencer StepSequencer_init(void) {
	return (StepSequencer) {
		.current_step = 0,
		.step_patterns = { 0 },
		.beat_clock = BeatClock_init(DEFAULT_TEMPO),
	};
}

bool StepSequencer_current_step_in_pattern_is_active(const StepSequencer* step_sequencer, uint8_t pattern) {
	if (pattern < STEP_SEQUENCER_NUM_STEP_PATTERNS) {
		return step_sequencer->step_patterns[pattern][step_sequencer->current_step];
	}
	return false;
}

void StepSequencer_increment_step(StepSequencer* step_sequencer) {
	step_sequencer->current_step = (step_sequencer->current_step + 1) % 16;
}
