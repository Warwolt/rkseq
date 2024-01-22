#include "sequencer/midi_control.h"

MidiControl MidiControl_init(void) {
	return (MidiControl) {
		.midi_clock_timeout_timer = MillisecondTimer_init(MIDI_CONTROL_CLOCK_TIMEOUT_MS),
	};
}

MidiControlEvents MidiControl_update(MidiControl* midi_control, uint8_t midi_byte) {
	MidiControlEvents events = { 0 };

	if (midi_byte == MIDI_CLOCK_BYTE) {
		events.switch_to_external_clock = true;
		MillisecondTimer_reset(&midi_control->midi_clock_timeout_timer);
	}

	if (MillisecondTimer_elapsed(&midi_control->midi_clock_timeout_timer)) {
		events.switch_to_internal_clock = true;
	}

	return events;
}
