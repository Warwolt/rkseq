#include <rktest/rktest.h>

#include "mock/mock_time.h"
#include "sequencer/midi_control.h"

#define EXPECT_MIDI_CONTROL_EVENTS_EQ(lhs, rhs)                            \
	EXPECT_EQ(lhs.switch_to_internal_clock, rhs.switch_to_internal_clock); \
	EXPECT_EQ(lhs.switch_to_external_clock, rhs.switch_to_external_clock);

TEST(MidiControl_tests, empty_input_gives_no_events) {
	MidiControl midi_control = MidiControl_init();

	const MidiControlEvents events = MidiControl_update(&midi_control, MIDI_NO_MSG_BYTE);

	const MidiControlEvents expected = { 0 };
	EXPECT_MIDI_CONTROL_EVENTS_EQ(events, expected);
}

TEST(MidiControl_tests, when_receiving_a_midi_clock_byte_then_switches_to_external_clock) {
	MidiControl midi_control = MidiControl_init();

	const MidiControlEvents events = MidiControl_update(&midi_control, MIDI_CLOCK_BYTE);

	EXPECT_TRUE(events.switch_to_external_clock);
}

TEST(MidiControl_tests, if_no_midi_clock_and_timeout_then_switch_to_internal_clock) {
	MidiControl midi_control = MidiControl_init();

	// send MIDI_CLOCK at time 0
	MockTime_set_now_ms(1234 + 0);
	MidiControl_update(&midi_control, MIDI_CLOCK_BYTE);

	// no clock received after timeout elapsed
	MockTime_set_now_ms(1234 + MIDI_CONTROL_CLOCK_TIMEOUT_MS);
	const MidiControlEvents events = MidiControl_update(&midi_control, MIDI_NO_MSG_BYTE);

	EXPECT_TRUE(events.switch_to_internal_clock);
}

TEST(MidiControl_tests, if_no_midi_clock_and_no_timeout_then_does_not_switch_to_internal_clock) {
	MidiControl midi_control = MidiControl_init();

	// send MIDI_CLOCK at time 0
	MockTime_set_now_ms(1234 + 0);
	MidiControl_update(&midi_control, MIDI_CLOCK_BYTE);

	// no clock received, but timeout has not elapsed yet
	MockTime_set_now_ms(1234 + MIDI_CONTROL_CLOCK_TIMEOUT_MS - 1);
	const MidiControlEvents events = MidiControl_update(&midi_control, MIDI_NO_MSG_BYTE);

	EXPECT_FALSE(events.switch_to_internal_clock);
}
