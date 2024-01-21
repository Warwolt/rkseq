#ifndef BEAT_CLOCK_H
#define BEAT_CLOCK_H

#include "util/microsecond_timer.h"

#include <stdbool.h>
#include <stdint.h>

#define BEAT_CLOCK_SEQUENCER_PPQN 96

typedef struct {
	bool is_playing;
	uint8_t beat_pulses;
	uint16_t tempo_bpm;
} BeatClock;

BeatClock BeatClock_init(uint8_t tempo_bpm);
void BeatClock_on_pulse(BeatClock* beat_clock);
void BeatClock_start(BeatClock* beat_clock);
void BeatClock_stop(BeatClock* beat_clock);
bool BeatClock_sixteenth_note_ready(const BeatClock* beat_clock);
bool BeatClock_midi_pulse_ready(const BeatClock* beat_clock);

#endif /* BEAT_CLOCK_H */
