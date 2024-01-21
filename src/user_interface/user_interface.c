#include "user_interface/user_interface.h"

UserInterface UserInterface_init(void) {
	return (UserInterface) { 0 };
}

UserInterfaceEvents UserInterface_update(UserInterface* ui, const UserInterfaceInput* input, const BeatClock* beat_clock) {
	UserInterfaceEvents events = { 0 };

	/* Set Tempo */
	if (input->RotaryEncoder_diff) {
		events.new_tempo_bpm = beat_clock->tempo_bpm + input->RotaryEncoder_diff;
	}

	/* Tempo display */
	// Display BPM
	const uint16_t number = beat_clock->tempo_bpm * 10;
	ui->display_digits[0] = number / 1 % 10;
	ui->display_digits[1] = number / 10 % 10;
	ui->display_digits[2] = number / 100 % 10;
	ui->display_digits[3] = number / 1000 % 10;

	return events;
}
