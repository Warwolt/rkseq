#include <rktest/rktest.h>

#include "sequencer/midi_control.h"

#define EXPECT_MIDI_CONTROL_EVENTS_EQ(lhs, rhs)                            \
	EXPECT_EQ(lhs.switch_to_internal_clock, rhs.switch_to_internal_clock); \
	EXPECT_EQ(lhs.switch_to_external_clock, rhs.switch_to_external_clock);

TEST(MidiControl_tests, empty_input_gives_no_events) {
	MidiControl midi_control = MidiControl_init();
	const uint8_t midi_byte = 0;

	const MidiControlEvents events = MidiControl_update(&midi_control, midi_byte);

	const MidiControlEvents expected = { 0 };
	EXPECT_MIDI_CONTROL_EVENTS_EQ(events, expected);
}
