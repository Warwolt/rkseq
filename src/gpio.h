#ifndef GPIO_H
#define GPIO_H

#include <stdbool.h>

typedef enum {
	PORT_B,
	PORT_C,
	PORT_D
} port_t;

typedef struct {
	port_t port;
	int num;
} pin_t;

typedef enum {
	PIN_MODE_INPUT,
	PIN_MODE_OUTPUT,
} pin_mode_t;

void pin_configure(pin_t pin, pin_mode_t mode);
void pin_write(pin_t pin, bool state);
void pin_set(pin_t pin);
void pin_clear(pin_t pin);

#endif /* GPIO_H */
