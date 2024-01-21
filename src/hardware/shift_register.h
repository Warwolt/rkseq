#ifndef SHIFT_REGISTER_H
#define SHIFT_REGISTER_H

#include "hardware/gpio.h"
#include "hardware/spi.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	spi_t spi;
	GpioPin latch_pin;
} ShiftRegister;

ShiftRegister ShiftRegister_init(spi_t spi, GpioPin latch_pin);
void ShiftRegister_read(const ShiftRegister* shift_reg, bool* out_buf, uint8_t out_buf_len);
void ShiftRegister_write(const ShiftRegister* shift_reg, uint8_t* bytes, uint8_t num_bytes);

#endif /* SHIFT_REGISTER_H */
