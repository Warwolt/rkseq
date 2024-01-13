#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#include "hardware/gpio.h"
#include "hardware/spi.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	spi_t spi;
	gpio_pin_t latch_pin;
} shift_register_t;

shift_register_t shift_register_init(spi_t spi, gpio_pin_t latch_pin);
void shift_register_read(const shift_register_t* shift_reg, bool* out_buf, uint8_t out_buf_len);
void shift_register_write(const shift_register_t* shift_reg, uint8_t* bytes, uint8_t num_bytes);

#endif /* SHIFT_REGISTER_H */
