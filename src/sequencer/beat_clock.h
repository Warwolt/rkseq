#ifndef BEAT_CLOCK_H
#define BEAT_CLOCK_H

#include "util/usec_timer.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t tempo_bpm;
	bool started;
	usec_timer_t timer;
	uint8_t beat_pulses;
} beat_clock_t;

beat_clock_t beat_clock_init(uint8_t tempo_bpm);
void beat_clock_on_pulse(beat_clock_t* beat_clock);
void beat_clock_set_tempo(beat_clock_t* beat_clock, uint8_t tempo_bpm);
void beat_clock_start(beat_clock_t* beat_clock);
void beat_clock_stop(beat_clock_t* beat_clock);
void beat_clock_update(beat_clock_t* beat_clock);
bool beat_clock_sixteenth_note_ready(const beat_clock_t* beat_clock);
bool beat_clock_midi_pulse_ready(const beat_clock_t* beat_clock);

#endif /* BEAT_CLOCK_H */
