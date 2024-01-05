#include "hardware/button.h"

static uint32_t button_debounce_state(button_t* button, button_state_t state, uint32_t time_now_ms) {
	if (time_now_ms - button->last_stable_signal_ms >= BUTTON_DEBOUNCE_MS) {
		button->last_stable_signal_ms = time_now_ms;
		return state;
	} else {
		return button->state;
	}
}

button_t button_init(void) {
	return (button_t) { 0 };
}

void button_update(button_t* button, button_state_t state, uint32_t time_now_ms) {
	const button_state_t debounced_state = button_debounce_state(button, state, time_now_ms);
	button->changed_now = button->state != debounced_state;
	button->state = debounced_state;
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
