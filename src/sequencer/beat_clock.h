#ifndef BEAT_CLOCK_H
#define BEAT_CLOCK_H

#include "util/usec_timer.h"

#include <stdbool.h>
#include <stdint.h>

#define BEAT_CLOCK_SEQUENCER_PPQN 96

typedef struct {
	bool is_playing;
	uint8_t beat_pulses;
	uint16_t tempo_bpm;
} beat_clock_t;

beat_clock_t beat_clock_init(uint8_t tempo_bpm);
void beat_clock_on_pulse(beat_clock_t* beat_clock);
void beat_clock_start(beat_clock_t* beat_clock);
void beat_clock_stop(beat_clock_t* beat_clock);
bool beat_clock_sixteenth_note_ready(const beat_clock_t* beat_clock);
bool beat_clock_midi_pulse_ready(const beat_clock_t* beat_clock);

#endif /* BEAT_CLOCK_H */
