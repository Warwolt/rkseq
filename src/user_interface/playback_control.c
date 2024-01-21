#include "user_interface/playback_control.h"

playback_control_events_t playback_control_update(const ui_devices_input_t* input, const beat_clock_t* beat_clock) {
	playback_control_events_t events = { 0 };

	/* Start / Stop Playback */
	if (input->start_button_pressed_now) {
		if (beat_clock->_is_playing) {
			events.start_playback = true;
		} else {
			events.stop_playback = true;
		}
	}

	/* Update Tempo */
	events.tempo_diff = input->rotary_diff;

	return events;
}
