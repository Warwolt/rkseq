#include "hardware/input_shift_register.h"

#include "hardware/spi.h"

input_shift_register_t input_shift_register_init(gpio_pin_t latch_pin) {
	gpio_pin_configure(latch_pin, PIN_MODE_OUTPUT);
	return (input_shift_register_t) {
		.latch_pin = latch_pin,
	};
}

uint16_t input_shift_register_read(const input_shift_register_t* shift_reg) {
	// load input state into shift register
	gpio_pin_clear(shift_reg->latch_pin);
	gpio_pin_set(shift_reg->latch_pin);

	// enable shift register clock and read states
	const uint8_t high_byte = spi_receive();
	const uint8_t low_byte = spi_receive();

	return (low_byte | high_byte << 8);
}
