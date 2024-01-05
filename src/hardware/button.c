#include "hardware/button.h"

// static uint32_t button_filter_state(button_t* button, button_state_t state, uint32_t time_now_ms) {
// 	const uint32_t state_stable = time_now_ms - button->last_stable_signal_ms >= BUTTON_DEBOUNCE_MS;
// 	button_state_t filtered_state;
// 	if (state_changed && state_stable) {
// 		filtered_state = state;
// 		button->last_stable_signal_ms = time_now_ms;
// 	} else {
// 		filtered_state = button->state;
// 	}
// }

button_t button_init(void) {
	return (button_t) { 0 };
}

void button_update(button_t* button, button_state_t state, uint32_t time_now_ms) {
	const bool state_changed = state != button->state;

	/* Debounce incoming signal */
	const uint32_t state_stable = time_now_ms - button->last_stable_signal_ms >= BUTTON_DEBOUNCE_MS;
	button_state_t filtered_state;
	if (state_changed && state_stable) {
		filtered_state = state;
		button->last_stable_signal_ms = time_now_ms;
	} else {
		filtered_state = button->state;
	}

	/* Update button state */
	button->changed_now = button->state != filtered_state;
	button->state = filtered_state;
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
