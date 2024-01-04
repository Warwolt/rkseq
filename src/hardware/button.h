#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

typedef enum {
	BUTTON_STATE_RELEASED = 0,
	BUTTON_STATE_PRESSED = 1,
} button_state_t;

typedef struct {
	button_state_t state;
	bool changed_now;
} button_t;

button_t button_init(void);
void button_update(button_t* button, button_state_t state);
bool button_just_pressed(const button_t* button);
bool button_just_relased(const button_t* button);
bool button_is_pressed(const button_t* button);
bool button_is_relased(const button_t* button);

#endif /* BUTTON_H */
