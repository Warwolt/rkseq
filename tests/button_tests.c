#include <rktest/rktest.h>

#include "hardware/button.h"

TEST(button_tests, button_initially_released) {
	button_t button = button_init();

	EXPECT_FALSE(button_is_pressed(&button));
	EXPECT_FALSE(button_just_pressed(&button));
}

TEST(button_tests, pressing_released_button_becomes_just_pressed) {
	button_t button = button_init();

	button_update(&button, BUTTON_STATE_PRESSED);

	EXPECT_TRUE(button_just_pressed(&button));
	EXPECT_TRUE(button_is_pressed(&button));
	EXPECT_FALSE(button_just_relased(&button));
	EXPECT_FALSE(button_is_relased(&button));
}

TEST(button_tests, pressing_pressed_button_remains_pressed) {
	button_t button = button_init();

	button_update(&button, BUTTON_STATE_PRESSED);
	button_update(&button, BUTTON_STATE_PRESSED);

	EXPECT_FALSE(button_just_pressed(&button));
	EXPECT_TRUE(button_is_pressed(&button));
	EXPECT_FALSE(button_just_relased(&button));
	EXPECT_FALSE(button_is_relased(&button));
}

TEST(button_tests, releasing_pressed_button_becomes_just_released) {
	button_t button = button_init();

	button_update(&button, BUTTON_STATE_PRESSED);
	button_update(&button, BUTTON_STATE_RELEASED);

	EXPECT_FALSE(button_just_pressed(&button));
	EXPECT_FALSE(button_is_pressed(&button));
	EXPECT_TRUE(button_just_relased(&button));
	EXPECT_TRUE(button_is_relased(&button));
}

TEST(button_tests, releasing_released_button_remains_released) {
	button_t button = button_init();

	button_update(&button, BUTTON_STATE_PRESSED);
	button_update(&button, BUTTON_STATE_RELEASED);
	button_update(&button, BUTTON_STATE_RELEASED);

	EXPECT_FALSE(button_just_pressed(&button));
	EXPECT_FALSE(button_is_pressed(&button));
	EXPECT_FALSE(button_just_relased(&button));
	EXPECT_TRUE(button_is_relased(&button));
}
