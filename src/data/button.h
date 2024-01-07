#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>
#include <stdint.h>

#define BUTTON_DEBOUNCE_MS 20

typedef enum {
	BUTTON_STATE_RELEASED = 0,
	BUTTON_STATE_PRESSED = 1,
} button_state_t;

typedef struct {
	button_state_t state;
	bool changed_now;
	uint32_t last_stable_signal_ms;
} button_t;

button_t button_init(void);
void button_update(button_t* button, button_state_t state, uint32_t time_now_ms);
bool button_just_pressed(const button_t* button);
bool button_just_relased(const button_t* button);
bool button_is_pressed(const button_t* button);
bool button_is_relased(const button_t* button);

#endif /* BUTTON_H */
