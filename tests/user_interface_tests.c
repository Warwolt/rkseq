#include <rktest/rktest.h>

#include "sequencer/step_sequencer.h"
#include "user_interface/user_interface.h"

TEST(UserInterface, shows_dashes_when_external_beat_clock_source) {
	const UserInterfaceInput input = { 0 };
	UserInterface user_interface = UserInterface_init();
	StepSequencer step_sequencer = StepSequencer_init();

	step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_EXTERNAL;
	UserInterface_update(&user_interface, &input, &step_sequencer);

	EXPECT_EQ(user_interface.segment_display_chars[0], '-');
	EXPECT_EQ(user_interface.segment_display_chars[1], '-');
	EXPECT_EQ(user_interface.segment_display_chars[2], '-');
	EXPECT_EQ(user_interface.segment_display_chars[3], '-');
}
