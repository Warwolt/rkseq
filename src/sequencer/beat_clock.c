#include "sequencer/beat_clock.h"

#include "util/math.h"

#define MIN_BPM 40
#define MAX_BPM 200

#define MIDI_PPQN 24

beat_clock_t beat_clock_init(uint8_t tempo_bpm) {
	return (beat_clock_t) {
		._tempo_bpm = clamp(tempo_bpm, MIN_BPM, MAX_BPM),
	};
}

void beat_clock_on_pulse(beat_clock_t* beat_clock) {
	beat_clock->_beat_pulses = (beat_clock->_beat_pulses + 1) % BEAT_CLOCK_SEQUENCER_PPQN;
}

void beat_clock_set_tempo(beat_clock_t* beat_clock, uint8_t tempo_bpm) {
	beat_clock->_tempo_bpm = clamp(tempo_bpm, MIN_BPM, MAX_BPM);
}

void beat_clock_start(beat_clock_t* beat_clock) {
	beat_clock->_started = true;
}

void beat_clock_stop(beat_clock_t* beat_clock) {
	beat_clock->_started = false;
}

void beat_clock_update(beat_clock_t* beat_clock) {
	(void)(beat_clock);
}

bool beat_clock_sixteenth_note_ready(const beat_clock_t* beat_clock) {
	return beat_clock->_beat_pulses == 0;
}

bool beat_clock_midi_pulse_ready(const beat_clock_t* beat_clock) {
	return (beat_clock->_beat_pulses % (BEAT_CLOCK_SEQUENCER_PPQN / MIDI_PPQN)) == 0;
}
