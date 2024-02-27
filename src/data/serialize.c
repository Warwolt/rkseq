#include "data/serialize.h"

void Serialize_pack_bits_into_bytes(bool* bits, size_t num_bits, uint8_t* bytes, size_t num_bytes, BitOrdering order) {
	for (size_t i = 0; i < num_bits && i < 8 * num_bytes; i++) {
		if (order == BIT_ORDERING_MSB_FIRST) {
			bytes[i / 8] |= (bits[i] & 0x1) << (i % 8);
		}

		if (order == BIT_ORDERING_LSB_FIRST) {
			bytes[i / 8] |= (bits[7 - i] & 0x1) << (i % 8);
		}
	}
}
