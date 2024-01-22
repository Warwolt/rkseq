#include <rktest/rktest.h>

#include "sequencer/midi_control.h"

TEST(MidiControl_tests, empty_input_gives_no_events) {
	MidiControl midi_control = MidiControl_init();
	const uint8_t midi_byte = 0;

	MidiControlEvents events = MidiControl_update(&midi_control, midi_byte);

	EXPECT_FALSE(events.switch_to_internal_clock);
	EXPECT_FALSE(events.switch_to_external_clock);
}
