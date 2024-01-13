#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#include "hardware/spi.h"
#include "hardware/gpio.h"

#include <stdbool.h>

typedef struct {
	spi_t spi;
	gpio_pin_t latch_pin;
} shift_register_t;

shift_register_t shift_register_init(spi_t spi, gpio_pin_t latch_pin);
void shift_register_read(const shift_register_t* shift_reg, bool* out_buf, uint8_t out_buf_len);

#endif /* SHIFT_REGISTER_H */
