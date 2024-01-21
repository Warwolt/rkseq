#include "user_interface/user_interface.h"

#include "util/math.h"

#define digit_to_string(digit) ('0' + digit)

UserInterface UserInterface_init(void) {
	return (UserInterface) { 0 };
}

UserInterfaceEvents UserInterface_update(UserInterface* ui, const UserInterfaceInput* input, const BeatClock* beat_clock) {
	UserInterfaceEvents events = { 0 };

	/* Set Tempo */
	if (input->RotaryEncoder_diff) {
		events.new_tempo_bpm = clamp(beat_clock->tempo_bpm + input->RotaryEncoder_diff, MIN_BPM, MAX_BPM);
	}

	/* Tempo display */
	// Display BPM
	const uint16_t number = beat_clock->tempo_bpm * 10;
	ui->segment_display_char[0] = digit_to_string(number / 1 % 10);
	ui->segment_display_char[1] = digit_to_string(number / 10 % 10);
	ui->segment_display_char[2] = digit_to_string(number / 100 % 10);
	ui->segment_display_char[3] = number > 999 ? digit_to_string(number / 1000 % 10) : ' ';

	return events;
}
