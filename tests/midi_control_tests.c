#include <rktest/rktest.h>

#include "hardware/timer0.h"
#include "mock/mock_time.h"
#include "sequencer/midi_control.h"

#define EXPECT_MIDI_CONTROL_EVENTS_EQ(lhs, rhs)                            \
	EXPECT_EQ(lhs.switch_to_internal_clock, rhs.switch_to_internal_clock); \
	EXPECT_EQ(lhs.switch_to_external_clock, rhs.switch_to_external_clock);

Timer0 MockTimer0_init() {
	return (Timer0) { .dummy = 0 };
}

TEST(MidiControl, empty_input_gives_no_events) {
	const Timer0 timer0 = MockTimer0_init();
	MidiControl midi_control = MidiControl_init(timer0);

	const MidiControlCommands events = MidiControl_update(&midi_control, MIDI_NO_MSG);

	const MidiControlCommands expected = { 0 };
	EXPECT_MIDI_CONTROL_EVENTS_EQ(events, expected);
}

TEST(MidiControl, when_receiving_a_midi_clock_byte_then_switches_to_external_clock) {
	const Timer0 timer0 = MockTimer0_init();
	MidiControl midi_control = MidiControl_init(timer0);

	const MidiControlCommands events = MidiControl_update(&midi_control, MIDI_TIMING_CLOCK);

	EXPECT_TRUE(events.switch_to_external_clock);
}

TEST(MidiControl, if_no_midi_clock_and_timeout_then_switch_to_internal_clock) {
	const Timer0 timer0 = MockTimer0_init();
	MidiControl midi_control = MidiControl_init(timer0);

	// send MIDI_TIMING_CLOCK at time 0
	MockTime_set_now_ms(1234 + 0);
	MidiControl_update(&midi_control, MIDI_TIMING_CLOCK);

	// no clock received after timeout elapsed
	MockTime_set_now_ms(1234 + MIDI_CONTROL_CLOCK_TIMEOUT_MS);
	const MidiControlCommands events = MidiControl_update(&midi_control, MIDI_NO_MSG);

	EXPECT_TRUE(events.switch_to_internal_clock);
}

TEST(MidiControl, if_no_midi_clock_and_no_timeout_then_does_not_switch_to_internal_clock) {
	const Timer0 timer0 = MockTimer0_init();
	MidiControl midi_control = MidiControl_init(timer0);

	// send MIDI_TIMING_CLOCK at time 0
	MockTime_set_now_ms(1234 + 0);
	MidiControl_update(&midi_control, MIDI_TIMING_CLOCK);

	// no clock received, but timeout has not elapsed yet
	MockTime_set_now_ms(1234 + MIDI_CONTROL_CLOCK_TIMEOUT_MS - 1);
	const MidiControlCommands events = MidiControl_update(&midi_control, MIDI_NO_MSG);

	EXPECT_FALSE(events.switch_to_internal_clock);
}
