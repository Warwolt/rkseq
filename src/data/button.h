#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

#define BUTTON_DEBOUNCE_MS 20

typedef enum {
	BUTTON_STATE_RELEASED = 0,
	BUTTON_STATE_PRESSED = 1,
} ButtonState;

typedef struct {
	ButtonState state;
	bool changed_now;
	uint32_t last_stable_signal_ms;
} Button;

void Button_update(Button* button, ButtonState state, uint32_t time_now_ms);
bool Button_just_pressed(const Button* button);
bool Button_just_relased(const Button* button);
bool Button_is_pressed(const Button* button);
bool Button_is_relased(const Button* button);

#endif /* BUTTON_H */
