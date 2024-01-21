#include "hardware/shift_register.h"

ShiftRegister ShiftRegister_init(spi_t spi, GpioPin latch_pin) {
	GpioPin_configure(latch_pin, PIN_MODE_OUTPUT);
	return (ShiftRegister) {
		.spi = spi,
		.latch_pin = latch_pin
	};
}

// Read bits from 74HC165
void ShiftRegister_read(const ShiftRegister* shift_reg, bool* out_buf, uint8_t out_buf_len) {
	// Update shift register content
	GpioPin_clear(shift_reg->latch_pin);
	GpioPin_set(shift_reg->latch_pin);

	// Read content
	uint8_t byte = 0;
	for (uint8_t i = 0; i < out_buf_len; i++) {
		if (i % 8 == 0) {
			byte = spi_receive(shift_reg->spi);
		}
		out_buf[i] = (byte >> i % 8) & 1;
	}
}

// Write bytes to 74HC595
void ShiftRegister_write(const ShiftRegister* shift_reg, uint8_t* bytes, uint8_t num_bytes) {
	for (uint8_t i = 0; i < num_bytes; i++) {
		spi_send(shift_reg->spi, bytes[i]);
	}
	GpioPin_clear(shift_reg->latch_pin);
	GpioPin_set(shift_reg->latch_pin);
}
