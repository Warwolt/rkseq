#include "beat_clock.h"

#include "util/math.h"

#define MIN_BPM 40
#define MAX_BPM 200

beat_clock_t beat_clock_init(uint8_t tempo_bpm) {
	return (beat_clock_t) {
		.tempo_bpm = clamp(tempo_bpm, MIN_BPM, MAX_BPM),
		.timer = usec_timer_init((60 * 1e6) / tempo_bpm)
	};
}

void beat_clock_set_tempo(beat_clock_t* beat_clock, uint8_t tempo_bpm) {
	beat_clock->tempo_bpm = clamp(tempo_bpm, MIN_BPM, MAX_BPM);
	beat_clock->timer.period_us = (60 * 1e6) / tempo_bpm;
}

void beat_clock_start(beat_clock_t* beat_clock) {
	beat_clock->is_playing = true;
	beat_clock->just_started = true;
}

void beat_clock_stop(beat_clock_t* beat_clock) {
	beat_clock->is_playing = false;
	beat_clock->just_started = false;
}

void beat_clock_update(beat_clock_t* beat_clock) {
	beat_clock->quarternote_elapsed = beat_clock->just_started || (beat_clock->is_playing && usec_timer_period_has_elapsed(&beat_clock->timer));
	beat_clock->just_started = false;
	if (beat_clock->quarternote_elapsed) {
		usec_timer_reset(&beat_clock->timer);
	}
}

bool beat_clock_should_output_quarternote(const beat_clock_t* beat_clock) {
	return beat_clock->quarternote_elapsed;
}
