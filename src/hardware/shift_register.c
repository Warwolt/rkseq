#include "hardware/shift_register.h"

shift_register_t shift_register_init(spi_t spi, gpio_pin_t latch_pin) {
	gpio_pin_configure(latch_pin, PIN_MODE_OUTPUT);
	return (shift_register_t) {
		.spi = spi,
		.latch_pin = latch_pin
	};
}

// Read bits from 74HC165
void shift_register_read(const shift_register_t* shift_reg, bool* out_buf, uint8_t out_buf_len) {
	// Update shift register content
	gpio_pin_clear(shift_reg->latch_pin);
	gpio_pin_set(shift_reg->latch_pin);

	// Read content
	uint8_t byte = 0;
	for (uint8_t i = 0; i < out_buf_len; i++) {
		if (i % 8 == 0) {
			byte = spi_receive(shift_reg->spi);
		}
		out_buf[i] = (byte >> i % 8) & 1;
	}
}
