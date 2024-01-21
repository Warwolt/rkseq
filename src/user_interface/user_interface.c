#include "user_interface/user_interface.h"

user_interface_t user_interface_init(void) {
	return (user_interface_t) { 0 };
}

user_interface_events_t user_interface_update(user_interface_t* ui, const user_interface_input_t* input, const beat_clock_t* beat_clock) {
	user_interface_events_t events = { 0 };

	/* Set Tempo */
	if (input->rotary_encoder_diff) {
		events.new_tempo_bpm = beat_clock->tempo_bpm + input->rotary_encoder_diff;
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
