#include "user_interface/user_interface.h"

#include "util/math.h"

#define digit_to_string(digit) ('0' + digit)

UserInterface UserInterface_init(void) {
	return (UserInterface) { 0 };
}

UserInterfaceEvents UserInterface_update(UserInterface* ui, const UserInterfaceInput* input, const StepSequencer* step_sequencer) {
	UserInterfaceEvents events = { 0 };

	/* Set Tempo */
	// FIXME: write a unit test that covers that tempo can only be changed if internal clock
	if (input->rotary_encoder_diff) {
		events.new_tempo_bpm = clamp(step_sequencer->beat_clock.tempo_bpm + input->rotary_encoder_diff, MIN_BPM, MAX_BPM);
	}

	/* Tempo display */
	switch (step_sequencer->beat_clock.source) {
		case BEAT_CLOCK_SOURCE_INTERNAL: {
			// Display BPM
			const uint16_t number = step_sequencer->beat_clock.tempo_bpm * 10;
			ui->segment_display_chars[0] = digit_to_string(number / 1 % 10);
			ui->segment_display_chars[1] = digit_to_string(number / 10 % 10);
			ui->segment_display_chars[2] = digit_to_string(number / 100 % 10);
			ui->segment_display_chars[3] = number > 999 ? digit_to_string(number / 1000 % 10) : ' ';

		} break;

		case BEAT_CLOCK_SOURCE_EXTERNAL: {
			ui->segment_display_chars[0] = '-';
			ui->segment_display_chars[1] = '-';
			ui->segment_display_chars[2] = '-';
			ui->segment_display_chars[3] = '-';
		} break;
	}

	return events;
}
