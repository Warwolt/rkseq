#include "hardware/button.h"

bool button_is_pressed(const button_t* button) {
	return button->is_pressed;
}

bool button_just_pressed(const button_t* button) {
	return button->changed_now && button->is_pressed;
}
