#include "hardware/shift_register.h"

ShiftRegister ShiftRegister_init(Spi spi, GpioPin latch_pin) {
	GpioPin_configure(latch_pin, PIN_MODE_OUTPUT);
	return (ShiftRegister) {
		.spi = spi,
		.latch_pin = latch_pin
	};
}

// Read bytes from 74HC165
void ShiftRegister_read(const ShiftRegister* shift_reg, uint8_t* byte_buf, uint8_t byte_buf_len) {
	// Update shift register content
	GpioPin_clear(shift_reg->latch_pin);
	GpioPin_set(shift_reg->latch_pin);

	// Read content
	for (uint8_t i = 0; i < byte_buf_len; i++) {
		byte_buf[i] = Spi_receive(shift_reg->spi);
	}
}

// Write bytes to 74HC595
void ShiftRegister_write(const ShiftRegister* shift_reg, uint8_t* byte_buf, uint8_t byte_buf_len) {
	// Write data to shift registers
	for (uint8_t i = 0; i < byte_buf_len; i++) {
		Spi_send(shift_reg->spi, byte_buf[i]);
	}

	// Push register content to outputs
	GpioPin_clear(shift_reg->latch_pin);
	GpioPin_set(shift_reg->latch_pin);
}
