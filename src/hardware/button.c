#include "hardware/button.h"

button_t button_init(void) {
	return (button_t) { 0 };
}

void button_update(button_t* button, button_state_t state) {
	button->changed_now = button->state != state;
	button->state = state;
}

bool button_is_pressed(const button_t* button) {
	return button->state == BUTTON_STATE_PRESSED;
}

bool button_just_pressed(const button_t* button) {
	return button->changed_now && button->state == BUTTON_STATE_PRESSED;
}

bool button_is_relased(const button_t* button) {
	return button->state == BUTTON_STATE_RELEASED;
}

bool button_just_relased(const button_t* button) {
	return button->changed_now && button->state == BUTTON_STATE_RELEASED;
}
