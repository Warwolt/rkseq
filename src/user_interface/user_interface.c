#include "user_interface/user_interface.h"

#include "util/math.h"

#define digit_to_string(digit) ('0' + digit)

UserInterface UserInterface_init(void) {
	return (UserInterface) { 0 };
}

UserInterfaceCommands UserInterface_update(UserInterface* ui, const UserInterfaceEvents* events, const StepSequencer* step_sequencer) {
	UserInterfaceCommands commands = { 0 };
	const BeatClockSource clock_source = step_sequencer->beat_clock.source;

	/* Set Tempo */
	if (clock_source == BEAT_CLOCK_SOURCE_INTERNAL) {
		commands.set_new_tempo_deci_bpm = clamp(step_sequencer->beat_clock.tempo_deci_bpm + 10 * events->rotary_encoder_diff, MIN_TEMPO, MAX_TEMPO);
	}

	/* Tempo display */
	switch (clock_source) {
		case BEAT_CLOCK_SOURCE_INTERNAL: {
			// Display BPM
			const uint16_t number = step_sequencer->beat_clock.tempo_deci_bpm;
			ui->segment_display_chars[0] = number > 999 ? digit_to_string(number / 1000 % 10) : ' ';
			ui->segment_display_chars[1] = digit_to_string(number / 100 % 10);
			ui->segment_display_chars[2] = digit_to_string(number / 10 % 10);
			ui->segment_display_chars[3] = digit_to_string(step_sequencer->beat_clock.tempo_deci_bpm % 10);
			ui->segment_display_period_enabled[2] = true;
		} break;

		case BEAT_CLOCK_SOURCE_EXTERNAL: {
			ui->segment_display_chars[0] = '-';
			ui->segment_display_chars[1] = '-';
			ui->segment_display_chars[2] = '-';
			ui->segment_display_chars[3] = '-';
			ui->segment_display_period_enabled[2] = false;
		} break;
	}

	return commands;
}
