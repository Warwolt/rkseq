#include "user_interface/playback_control.h"

static void toggle_beat_clock(beat_clock_t* beat_clock) {
	if (!beat_clock->started) {
		beat_clock_start(beat_clock);
	} else {
		beat_clock_stop(beat_clock);
	}
}

void playback_control_update(ui_devices_t* ui_devices, beat_clock_t* beat_clock) {
	/* Start / Stop Playback */
	if (button_just_pressed(&ui_devices->start_button)) {
		toggle_beat_clock(beat_clock);
	}

	/* Update Tempo */
	const int rotary_diff = rotary_encoder_read(&ui_devices->encoder);
	beat_clock_set_tempo(beat_clock, beat_clock->tempo_bpm + rotary_diff);

	/* Display Current Tempo*/
	segment_display_set_number(&ui_devices->display, beat_clock->tempo_bpm * 10);
	segment_display_enable_period(&ui_devices->display, 1);
}
