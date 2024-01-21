#include "sequencer/beat_clock.h"

#include "util/math.h"

#define MIDI_PPQN 24

beat_clock_t beat_clock_init(uint8_t tempo_bpm) {
	return (beat_clock_t) {
		.beat_pulses = 0,
		.is_playing = false,
		.tempo_bpm = tempo_bpm
	};
}

void beat_clock_on_pulse(beat_clock_t* beat_clock) {
	beat_clock->beat_pulses = (beat_clock->beat_pulses + 1) % BEAT_CLOCK_SEQUENCER_PPQN;
}

void beat_clock_start(beat_clock_t* beat_clock) {
	beat_clock->is_playing = true;
}

void beat_clock_stop(beat_clock_t* beat_clock) {
	beat_clock->is_playing = false;
}

bool beat_clock_sixteenth_note_ready(const beat_clock_t* beat_clock) {
	return beat_clock->beat_pulses == 0;
}

bool beat_clock_midi_pulse_ready(const beat_clock_t* beat_clock) {
	return (beat_clock->beat_pulses % (BEAT_CLOCK_SEQUENCER_PPQN / MIDI_PPQN)) == 0;
}
