#include "data/serialize.h"

#include <stdio.h> // debugging

// The natural numbers sliced into reversed sequences of 8
// [7, 6, 5, 4, 3, 2, 1, 0] [15, 14, 13, 12, 11, 10, 9, 8] [23, 22, 21, 20, 19, 18, 17, 16]
static size_t lsb_index(size_t i) {
	// intentionally truncating division
	return (i / 8 + 1) * 8 - 1 - (i % 8);
}

void Serialize_pack_bits_into_bytes(bool* bits, size_t num_bits, uint8_t* bytes, size_t num_bytes, BitOrdering order) {
	for (size_t i = 0; (i < num_bits) && (i < 8 * num_bytes); i++) {
		const size_t bit_index = order == BIT_ORDERING_LSB_FIRST ? lsb_index(i) : i;
		bytes[i / 8] |= (bits[bit_index] & 0x1) << (i % 8);
	}
}
