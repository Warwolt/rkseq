#include <rktest/rktest.h>

#include "sequencer/step_sequencer.h"
#include "user_interface/user_interface.h"
#include "util/math.h"

#define EXPECT_ARRAY_EQ(lhs, rhs, len)                 \
	for (int i = 0; i < len; i++) {                    \
		EXPECT_EQ_INFO(lhs[i], rhs[i], "i = %d\n", i); \
	}

#define EXPECT_CHAR_ARRAY_EQ(lhs, rhs, len)                 \
	for (int i = 0; i < len; i++) {                         \
		EXPECT_CHAR_EQ_INFO(lhs[i], rhs[i], "i = %d\n", i); \
	}

TEST(UserInterface, shows_dashes_on_display_when_beat_clock_source_is_external) {
	const UserInterfaceEvents input = { 0 };
	UserInterface user_interface = UserInterface_init();
	StepSequencer step_sequencer = StepSequencer_init();

	// should show BPM
	step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_INTERNAL;
	UserInterface_update(&user_interface, &input, &step_sequencer);

	// should now switch to just dashes
	step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_EXTERNAL;
	UserInterface_update(&user_interface, &input, &step_sequencer);

	const char expected_chars[5] = "----";
	const bool expected_periods[4] = { false, false, false, false };
	EXPECT_CHAR_ARRAY_EQ(user_interface.segment_display_chars, expected_chars, 4);
	EXPECT_ARRAY_EQ(user_interface.segment_display_period_enabled, expected_periods, 4);
}

TEST(UserInterface, shows_bpm_on_display_when_beat_clock_source_is_internal) {
	const UserInterfaceEvents input = { 0 };
	UserInterface user_interface = UserInterface_init();
	StepSequencer step_sequencer = StepSequencer_init();

	// set BPM to 120.3
	step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_INTERNAL;
	step_sequencer.beat_clock.tempo_deci_bpm = 1203;
	UserInterface_update(&user_interface, &input, &step_sequencer);

	const char expected_chars[5] = "1203";
	const bool expected_periods[4] = { false, false, true, false };
	EXPECT_CHAR_ARRAY_EQ(user_interface.segment_display_chars, expected_chars, 4);
	EXPECT_ARRAY_EQ(user_interface.segment_display_period_enabled, expected_periods, 4);
}

TEST(UserInterface, rotary_encoder_changes_tempo_when_clock_source_is_internal) {
	const UserInterfaceEvents input = { .rotary_encoder_diff = 1 };
	UserInterface user_interface = UserInterface_init();
	StepSequencer step_sequencer = StepSequencer_init();

	step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_INTERNAL;
	step_sequencer.beat_clock.tempo_deci_bpm = 1200;
	UserInterfaceCommands commands = UserInterface_update(&user_interface, &input, &step_sequencer);

	EXPECT_EQ(commands.set_new_tempo_deci_bpm, 1210);
}

TEST(UserInterface, rotary_encoder_does_not_change_tempo_when_clock_source_is_external) {
	const UserInterfaceEvents input = { .rotary_encoder_diff = 1 };
	UserInterface user_interface = UserInterface_init();
	StepSequencer step_sequencer = StepSequencer_init();

	step_sequencer.beat_clock.source = BEAT_CLOCK_SOURCE_EXTERNAL;
	step_sequencer.beat_clock.tempo_deci_bpm = 1200;
	UserInterfaceCommands commands = UserInterface_update(&user_interface, &input, &step_sequencer);

	EXPECT_EQ(commands.set_new_tempo_deci_bpm, 0);
}
