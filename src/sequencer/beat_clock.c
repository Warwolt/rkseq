#include "sequencer/beat_clock.h"

#include "util/math.h"

#define MIDI_PPQN 24

BeatClock BeatClock_init(uint16_t tempo_deci_bpm) {
	return (BeatClock) {
		.beat_pulses = 0,
		.is_playing = false,
		.tempo_deci_bpm = tempo_deci_bpm,
		.source = BEAT_CLOCK_SOURCE_INTERNAL,
	};
}

void BeatClock_count_pulse(BeatClock* beat_clock) {
	beat_clock->beat_pulses = (beat_clock->beat_pulses + 1) % BEAT_CLOCK_SEQUENCER_PPQN;
}

void BeatClock_start(BeatClock* beat_clock) {
	beat_clock->is_playing = true;
}

void BeatClock_stop(BeatClock* beat_clock) {
	beat_clock->is_playing = false;
}

bool BeatClock_sixteenth_note_ready(const BeatClock* beat_clock) {
	return (beat_clock->beat_pulses % (BEAT_CLOCK_SEQUENCER_PPQN / 4)) == 0;
}

bool BeatClock_midi_pulse_ready(const BeatClock* beat_clock) {
	return (beat_clock->beat_pulses % (BEAT_CLOCK_SEQUENCER_PPQN / MIDI_PPQN)) == 0;
}
