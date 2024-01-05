#include <rktest/rktest.h>

#include "hardware/button.h"

static uint32_t g_time_now;

static void update_button_with_debounce(button_t* button, button_state_t state) {
	button_update(button, BUTTON_STATE_PRESSED, g_time_now);
	g_time_now += BUTTON_DEBOUNCE_MS;
	button_update(button, BUTTON_STATE_PRESSED, g_time_now);
}

TEST_SETUP(button_tests) {
	g_time_now = 0;
}

TEST(button_tests, button_initially_released) {
	button_t button = button_init();

	EXPECT_FALSE(button_is_pressed(&button));
	EXPECT_FALSE(button_just_pressed(&button));
}

TEST(button_tests, pressing_released_button_becomes_just_pressed) {
	button_t button = button_init();

	update_button_with_debounce(&button, BUTTON_STATE_PRESSED);

	EXPECT_TRUE(button_just_pressed(&button));
	EXPECT_TRUE(button_is_pressed(&button));
	EXPECT_FALSE(button_just_relased(&button));
	EXPECT_FALSE(button_is_relased(&button));
}

// TEST(button_tests, pressing_pressed_button_remains_pressed) {
// 	button_t button = button_init();

// 	button_update(&button, BUTTON_STATE_PRESSED);
// 	button_update(&button, BUTTON_STATE_PRESSED);

// 	EXPECT_FALSE(button_just_pressed(&button));
// 	EXPECT_TRUE(button_is_pressed(&button));
// 	EXPECT_FALSE(button_just_relased(&button));
// 	EXPECT_FALSE(button_is_relased(&button));
// }

// TEST(button_tests, releasing_pressed_button_becomes_just_released) {
// 	button_t button = button_init();

// 	button_update(&button, BUTTON_STATE_PRESSED);
// 	button_update(&button, BUTTON_STATE_RELEASED);

// 	EXPECT_FALSE(button_just_pressed(&button));
// 	EXPECT_FALSE(button_is_pressed(&button));
// 	EXPECT_TRUE(button_just_relased(&button));
// 	EXPECT_TRUE(button_is_relased(&button));
// }

// TEST(button_tests, releasing_released_button_remains_released) {
// 	button_t button = button_init();

// 	button_update(&button, BUTTON_STATE_PRESSED);
// 	button_update(&button, BUTTON_STATE_RELEASED);
// 	button_update(&button, BUTTON_STATE_RELEASED);

// 	EXPECT_FALSE(button_just_pressed(&button));
// 	EXPECT_FALSE(button_is_pressed(&button));
// 	EXPECT_FALSE(button_just_relased(&button));
// 	EXPECT_TRUE(button_is_relased(&button));
// }

TEST(button_tests, released_button_bouncing_while_pressed_remains_released) {
	button_t button = button_init();

	button_update(&button, BUTTON_STATE_PRESSED, 0);
	button_update(&button, BUTTON_STATE_RELEASED, 1);
	button_update(&button, BUTTON_STATE_PRESSED, 2);

	EXPECT_TRUE(button_is_relased(&button));
	EXPECT_FALSE(button_is_pressed(&button));
}

TEST(button_tests, pressed_button_bouncing_while_released_remains_pressed) {
	button_t button = button_init();

	update_button_with_debounce(&button, BUTTON_STATE_PRESSED);
	button_update(&button, BUTTON_STATE_RELEASED, g_time_now + 1);
	button_update(&button, BUTTON_STATE_PRESSED, g_time_now + 2);
	button_update(&button, BUTTON_STATE_RELEASED, g_time_now + 3);

	EXPECT_FALSE(button_is_relased(&button));
	EXPECT_TRUE(button_is_pressed(&button));
}
