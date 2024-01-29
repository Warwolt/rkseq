#ifndef MIDI_CONTROL_H
#define MIDI_CONTROL_H

#include "data/midi.h"
#include "hardware/timer0.h"
#include "util/timer.h"

#include <stdbool.h>

#define MIDI_CONTROL_CLOCK_TIMEOUT_MS 1000

typedef struct {
	bool switch_to_internal_clock;
	bool switch_to_external_clock;
} MidiControlEvents;

typedef struct {
	MillisecondTimer midi_clock_timeout_timer;
} MidiControl;

MidiControl MidiControl_init(Timer0 timer0);
MidiControlEvents MidiControl_update(MidiControl* midi_control, uint8_t midi_byte);

#endif /* MIDI_CONTROL_H */
