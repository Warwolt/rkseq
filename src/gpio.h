#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef struct {
	volatile uint8_t* port;
	int num;
} pin_t;

typedef enum {
	PIN_MODE_INPUT,
	PIN_MODE_OUTPUT,
} pin_mode_t;

typedef enum {
	PIN_STATE_SET = 0,
	PIN_STATE_CLEAR = 1,
} pin_state_t;

void pin_configure(pin_t pin, pin_mode_t mode);
void pin_write(pin_t pin, pin_state_t state);
pin_state_t pin_read(pin_t pin);

#endif /* GPIO_H */
