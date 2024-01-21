#include "user_interface/playback_ui.h"

playback_ui_t playback_ui_init(void) {
	return (playback_ui_t) { 0 };
}

playback_ui_events_t playback_ui_update(playback_ui_t* playback_ui, const playback_ui_input_t* input, const beat_clock_t* beat_clock) {
	playback_ui_events_t events = { 0 };

	/* Set Tempo */
	if (input->rotary_encoder_diff) {
		events.new_tempo_bpm = beat_clock->tempo_bpm + input->rotary_encoder_diff;
	}

	/* Tempo display */
	// Display BPM
	const uint16_t number = beat_clock->tempo_bpm * 10;
	playback_ui->display_digits[0] = number / 1 % 10;
	playback_ui->display_digits[1] = number / 10 % 10;
	playback_ui->display_digits[2] = number / 100 % 10;
	playback_ui->display_digits[3] = number / 1000 % 10;

	return events;
}
