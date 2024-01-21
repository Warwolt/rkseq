#include <rktest/rktest.h>

#include "data/button.h"

static uint32_t g_time_now;

static void debounced_Button_update(Button* button, ButtonState state) {
	Button_update(button, state, g_time_now);
	g_time_now += BUTTON_DEBOUNCE_MS;
	Button_update(button, state, g_time_now);
}

TEST_SETUP(Button_tests) {
	g_time_now = 0;
}

TEST(Button_tests, Button_initially_released) {
	Button button = { 0 };

	EXPECT_FALSE(Button_is_pressed(&button));
	EXPECT_FALSE(Button_just_pressed(&button));
}

TEST(Button_tests, pressing_released_Button_becomes_just_pressed) {
	Button button = { 0 };

	debounced_Button_update(&button, BUTTON_STATE_PRESSED);

	EXPECT_TRUE(Button_just_pressed(&button));
	EXPECT_TRUE(Button_is_pressed(&button));
	EXPECT_FALSE(Button_just_relased(&button));
	EXPECT_FALSE(Button_is_relased(&button));
}

TEST(Button_tests, pressing_pressed_Button_remains_pressed) {
	Button button = { 0 };

	debounced_Button_update(&button, BUTTON_STATE_PRESSED);
	debounced_Button_update(&button, BUTTON_STATE_PRESSED);

	EXPECT_FALSE(Button_just_pressed(&button));
	EXPECT_TRUE(Button_is_pressed(&button));
	EXPECT_FALSE(Button_just_relased(&button));
	EXPECT_FALSE(Button_is_relased(&button));
}

TEST(Button_tests, releasing_pressed_Button_becomes_just_released) {
	Button button = { 0 };

	debounced_Button_update(&button, BUTTON_STATE_PRESSED);
	debounced_Button_update(&button, BUTTON_STATE_RELEASED);

	EXPECT_FALSE(Button_just_pressed(&button));
	EXPECT_FALSE(Button_is_pressed(&button));
	EXPECT_TRUE(Button_just_relased(&button));
	EXPECT_TRUE(Button_is_relased(&button));
}

TEST(Button_tests, releasing_released_Button_remains_released) {
	Button button = { 0 };

	debounced_Button_update(&button, BUTTON_STATE_PRESSED);
	debounced_Button_update(&button, BUTTON_STATE_RELEASED);
	debounced_Button_update(&button, BUTTON_STATE_RELEASED);

	EXPECT_FALSE(Button_just_pressed(&button));
	EXPECT_FALSE(Button_is_pressed(&button));
	EXPECT_FALSE(Button_just_relased(&button));
	EXPECT_TRUE(Button_is_relased(&button));
}

TEST(Button_tests, released_Button_bouncing_while_pressed_remains_released) {
	Button button = { 0 };

	Button_update(&button, BUTTON_STATE_PRESSED, 0);
	Button_update(&button, BUTTON_STATE_RELEASED, 1);
	Button_update(&button, BUTTON_STATE_PRESSED, 2);

	EXPECT_TRUE(Button_is_relased(&button));
	EXPECT_FALSE(Button_is_pressed(&button));
}

TEST(Button_tests, pressed_Button_bouncing_while_released_remains_pressed) {
	Button button = { 0 };

	debounced_Button_update(&button, BUTTON_STATE_PRESSED);
	Button_update(&button, BUTTON_STATE_RELEASED, g_time_now + 1);
	Button_update(&button, BUTTON_STATE_PRESSED, g_time_now + 2);
	Button_update(&button, BUTTON_STATE_RELEASED, g_time_now + 3);

	EXPECT_FALSE(Button_is_relased(&button));
	EXPECT_TRUE(Button_is_pressed(&button));
}
