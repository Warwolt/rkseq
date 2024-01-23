#include <rktest/rktest.h>

#include "sequencer/step_sequencer.h"
#include "user_interface/user_interface.h"

TEST(UserInterface, shows_dashes_on_display_when_beat_clock_source_is_external) {
	const UserInterfaceInput input = { 0 };
	UserInterface user_interface = UserInterface_init();
	StepSequencer step_sequencer = StepSequencer_init();

	step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_EXTERNAL;
	UserInterface_update(&user_interface, &input, &step_sequencer);

	EXPECT_CHAR_EQ(user_interface.segment_display_chars[0], '-');
	EXPECT_CHAR_EQ(user_interface.segment_display_chars[1], '-');
	EXPECT_CHAR_EQ(user_interface.segment_display_chars[2], '-');
	EXPECT_CHAR_EQ(user_interface.segment_display_chars[3], '-');
}

TEST(UserInterface, shows_bpm_on_display_when_beat_clock_source_is_internal) {
	const UserInterfaceInput input = { 0 };
	UserInterface user_interface = UserInterface_init();
	StepSequencer step_sequencer = StepSequencer_init();

	// set BPM to 120.3
	step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_INTERNAL;
	step_sequencer.beat_clock.tempo_bpm = 120;
	step_sequencer.beat_clock.tempo_deci_bpm = 3;
	UserInterface_update(&user_interface, &input, &step_sequencer);

	EXPECT_CHAR_EQ(user_interface.segment_display_chars[0], '3');
	EXPECT_CHAR_EQ(user_interface.segment_display_chars[1], '0');
	EXPECT_CHAR_EQ(user_interface.segment_display_chars[2], '2');
	EXPECT_CHAR_EQ(user_interface.segment_display_chars[3], '1');
	EXPECT_TRUE(user_interface.segment_display_period_enabled[1]);
}
