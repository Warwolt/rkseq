#include "sequencer/beat_clock.h"

#include "util/math.h"

#define MIN_BPM 40
#define MAX_BPM 200

#define SEQUENCER_PPQN 96
#define MIDI_PPQN 24

beat_clock_t beat_clock_init(uint8_t tempo_bpm) {
	return (beat_clock_t) {
		.tempo_bpm = clamp(tempo_bpm, MIN_BPM, MAX_BPM),
		.timer = usec_timer_init((60 * 1e6) / tempo_bpm)
	};
}

void beat_clock_on_pulse(beat_clock_t* beat_clock) {
	beat_clock->beat_pulses = (beat_clock->beat_pulses + 1) % SEQUENCER_PPQN;
}

void beat_clock_set_tempo(beat_clock_t* beat_clock, uint8_t tempo_bpm) {
	beat_clock->tempo_bpm = clamp(tempo_bpm, MIN_BPM, MAX_BPM);
	beat_clock->timer.period_us = (60 * 1e6) / tempo_bpm;
}

void beat_clock_start(beat_clock_t* beat_clock) {
	beat_clock->started = true;
}

void beat_clock_stop(beat_clock_t* beat_clock) {
	beat_clock->started = false;
}

void beat_clock_update(beat_clock_t* beat_clock) {
	(void)(beat_clock);
}

bool beat_clock_sixteenth_note_ready(const beat_clock_t* beat_clock) {
	return beat_clock->beat_pulses == 0;
}

bool beat_clock_midi_pulse_ready(const beat_clock_t* beat_clock) {
	return (beat_clock->beat_pulses % (SEQUENCER_PPQN / MIDI_PPQN)) == 0;
}
