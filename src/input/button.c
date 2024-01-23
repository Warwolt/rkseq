#include "input/button.h"

static uint32_t debounce_input_state(Button* button, ButtonState input_state, uint32_t time_now_ms) {
	if (time_now_ms - button->last_stable_signal_ms >= BUTTON_DEBOUNCE_MS) {
		button->last_stable_signal_ms = time_now_ms;
		return input_state;
	} else {
		return button->state;
	}
}

Button Button_init(void) {
	return (Button) { 0 };
}

void Button_update(Button* button, ButtonState state, uint32_t time_now_ms) {
	const ButtonState debounced_state = debounce_input_state(button, state, time_now_ms);
	button->changed_now = button->state != debounced_state;
	button->state = debounced_state;
}

bool Button_is_pressed(const Button* button) {
	return button->state == BUTTON_STATE_PRESSED;
}

bool Button_just_pressed(const Button* button) {
	return button->changed_now && button->state == BUTTON_STATE_PRESSED;
}

bool Button_is_relased(const Button* button) {
	return button->state == BUTTON_STATE_RELEASED;
}

bool Button_just_relased(const Button* button) {
	return button->changed_now && button->state == BUTTON_STATE_RELEASED;
}
