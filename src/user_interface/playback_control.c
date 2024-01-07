#include "user_interface/playback_control.h"

static void toggle_beat_clock(beat_clock_t* beat_clock) {
	if (!beat_clock->is_playing) {
		beat_clock_start(beat_clock);
	} else {
		beat_clock_stop(beat_clock);
	}
}

void playback_control_update(playback_control_t* playback_control, beat_clock_t* beat_clock) {
	/* Start / Stop Playback */
	if (button_just_pressed(&playback_control->start_button)) {
		toggle_beat_clock(beat_clock);
	}

	/* Update Tempo */
	const int rotary_diff = rotary_encoder_read(&playback_control->tempo_knob);
	beat_clock_set_tempo(beat_clock, beat_clock->tempo_bpm + rotary_diff);

	/* Display Current Tempo*/
	segment_display_set_number(&playback_control->tempo_display, beat_clock->tempo_bpm * 10);
	segment_display_enable_period(&playback_control->tempo_display, 1);
	segment_display_update(&playback_control->tempo_display);
}
