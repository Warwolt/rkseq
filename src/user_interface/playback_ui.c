#include "user_interface/playback_ui.h"

playback_ui_t playback_ui_init(void) {
	return (playback_ui_t) { 0 };
}

playback_ui_events_t playback_ui_update(playback_ui_t* playback_ui, const playback_ui_input_t* input, const beat_clock_t* beat_clock) {
	playback_ui_events_t events = { 0 };

	/* Start / Stop Playback */
	if (input->start_button_pressed_now) {
		if (beat_clock->_is_playing) {
			events.start_playback = true;
		} else {
			events.stop_playback = true;
		}
	}

	/* Update Tempo */

	events.tempo_diff = input->rotary_encoder_diff;

	return events;
}
