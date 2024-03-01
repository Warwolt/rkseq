#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
	BIT_ORDERING_MSB_FIRST,
	BIT_ORDERING_LSB_FIRST,
} BitOrdering;

void Serialize_pack_bits_into_bytes(bool* bits, size_t num_bits, uint8_t* bytes, size_t num_bytes, BitOrdering order);

#endif /* SERIALIZE_H */
