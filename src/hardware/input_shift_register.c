#include "hardware/input_shift_register.h"

#include "hardware/spi.h"

input_shift_register_t input_shift_register_init(gpio_pin_t load_pin, gpio_pin_t enable_pin) {
	gpio_pin_configure(load_pin, PIN_MODE_OUTPUT);
	gpio_pin_configure(enable_pin, PIN_MODE_OUTPUT);
	return (input_shift_register_t) {
		.load_pin = load_pin,
		.enable_pin = enable_pin,
	};
}

uint16_t input_shift_register_read(const input_shift_register_t* shift_reg) {
	// load input state into shift register
	gpio_pin_clear(shift_reg->load_pin);
	gpio_pin_set(shift_reg->load_pin);

	// enable shift register clock and read states
	gpio_pin_clear(shift_reg->enable_pin);
	const uint8_t high_byte = spi_receive();
	const uint8_t low_byte = spi_receive();
	gpio_pin_set(shift_reg->enable_pin);

	return (low_byte | high_byte << 8);
}
