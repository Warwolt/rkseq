#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef struct {
	volatile uint8_t* port;
	int num;
} gpio_pin_t;

typedef enum {
	PIN_MODE_INPUT,
	PIN_MODE_OUTPUT,
} pin_mode_t;

typedef enum {
	PIN_STATE_SET = 0,
	PIN_STATE_CLEAR = 1,
} pin_state_t;

void gpio_pin_configure(gpio_pin_t pin, pin_mode_t mode);
void gpio_pin_write(gpio_pin_t pin, pin_state_t state);
pin_state_t gpio_pin_read(gpio_pin_t pin);

#endif /* GPIO_H */
