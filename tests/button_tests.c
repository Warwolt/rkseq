#include <rktest/rktest.h>

#include "hardware/button.h"

TEST(button_tests, button_initially_released) {
	button_t button = { 0 };

	EXPECT_FALSE(button_is_pressed(&button));
	EXPECT_FALSE(button_just_pressed(&button));
}
