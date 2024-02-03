#include "sequencer/midi_control.h"

MidiControl MidiControl_init(Timer0 timer0) {
	return (MidiControl) {
		.midi_clock_timeout_timer = MillisecondTimer_init(timer0, MIDI_CONTROL_CLOCK_TIMEOUT_MS),
	};
}

MidiControlCommands MidiControl_update(MidiControl* midi_control, uint8_t midi_byte) {
	MidiControlCommands commands = { 0 };

	if (midi_byte == MIDI_TIMING_CLOCK) {
		commands.switch_to_external_clock = true;
		MillisecondTimer_reset(&midi_control->midi_clock_timeout_timer);
	}

	if (MillisecondTimer_elapsed(&midi_control->midi_clock_timeout_timer)) {
		commands.switch_to_internal_clock = true;
	}

	return commands;
}
