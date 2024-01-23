#ifndef BEAT_CLOCK_H
#define BEAT_CLOCK_H

#include "util/timer.h"

#include <stdbool.h>
#include <stdint.h>

#define BEAT_CLOCK_SEQUENCER_PPQN 96
#define DEFAULT_BPM 120
#define MIN_BPM 40
#define MAX_BPM 200

typedef enum {
	BEAT_CLOCK_SOURCE_INTERNAL,
	BEAT_CLOCK_SOURCE_EXTERNAL,
} BeatClockSource;

typedef struct {
	bool is_playing;
	uint8_t beat_pulses;
	uint16_t tempo_bpm;
	uint8_t tempo_centi_bpm;
	BeatClockSource source;
} BeatClock;

BeatClock BeatClock_init(uint8_t tempo_bpm);
void BeatClock_on_pulse(BeatClock* beat_clock);
void BeatClock_start(BeatClock* beat_clock);
void BeatClock_stop(BeatClock* beat_clock);
bool BeatClock_sixteenth_note_ready(const BeatClock* beat_clock);
bool BeatClock_midi_pulse_ready(const BeatClock* beat_clock);

#endif /* BEAT_CLOCK_H */
