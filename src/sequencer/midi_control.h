#ifndef MIDI_CONTROL_H
#define MIDI_CONTROL_H

#include "util/timer.h"

#include <stdbool.h>

#define MIDI_CONTROL_CLOCK_TIMEOUT_MS 1000

#define MIDI_NO_MSG_BYTE 0x00
#define MIDI_CLOCK_BYTE 0xF8
#define MIDI_START_BYTE 0xFA
#define MIDI_CONTINUE_BYTE 0xFB
#define MIDI_STOP_BYTE 0xFC

typedef struct {
	bool switch_to_internal_clock;
	bool switch_to_external_clock;
} MidiControlEvents;

typedef struct {
	MillisecondTimer midi_clock_timeout_timer;
} MidiControl;

MidiControl MidiControl_init(void);
MidiControlEvents MidiControl_update(MidiControl* midi_control, uint8_t midi_byte);

#endif /* MIDI_CONTROL_H */
