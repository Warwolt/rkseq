#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

typedef struct {
	volatile uint8_t* port;
	volatile uint8_t* in_reg;
	uint8_t num;
} GpioPin;

typedef enum {
	PIN_MODE_INPUT,
	PIN_MODE_OUTPUT,
} PinMode;

typedef enum {
	PIN_STATE_LOW = 0,
	PIN_STATE_HIGH = 1,
} PinState;

GpioPin GpioPin_init(volatile uint8_t* port, uint8_t pin_num);
GpioPin GpioPin_init_mode(volatile uint8_t* port, uint8_t pin_num, PinMode mode);
void GpioPin_configure(GpioPin pin, PinMode mode);
void GpioPin_write(GpioPin pin, PinState state);
void GpioPin_toggle(GpioPin pin);
PinState GpioPin_read(GpioPin pin);

static inline void GpioPin_set(GpioPin pin) { GpioPin_write(pin, PIN_STATE_HIGH); }
static inline void GpioPin_clear(GpioPin pin) { GpioPin_write(pin, PIN_STATE_LOW); }

#endif /* GPIO_H */
