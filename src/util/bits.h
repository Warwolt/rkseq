#ifndef BITS_H
#define BITS_H

#include <avr/io.h>

#define clear_bit(reg, bit) (_SFR_BYTE(reg) &= ~_BV(bit))
#define set_bit(reg, bit) (_SFR_BYTE(reg) |= _BV(bit))
#define toggle_bit(reg, bit) (_SFR_BYTE(reg) ^= _BV(bit))
#define write_bit(reg, bit, value) \
	if (value)                     \
		set_bit(reg, bit);         \
	else                           \
		clear_bit(reg, bit)

#endif /* BITS_H */
