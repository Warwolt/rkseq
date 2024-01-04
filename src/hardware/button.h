#ifndef BUTTON_H
#define BUTTON_H

#include <stdbool.h>

typedef struct {
	bool is_pressed;
	bool changed_now;
} button_t;

bool button_is_pressed(const button_t* button);
bool button_just_pressed(const button_t* button);

#endif /* BUTTON_H */
