#include <rktest/rktest.h>

#include "data/serialize.h"
#include "test_assertions.h"

#include <stdbool.h>

TEST(serialize_tests, serializing_empty_bit_array_gives_empty_byte_array) {
	bool bit_array[16] = { 0 };
	uint8_t byte_array[2] = { 0 };

	Serialize_pack_bits_into_bytes(bit_array, arraylen(bit_array), byte_array, arraylen(byte_array), BIT_ORDERING_LSB_FIRST);

	const uint8_t expected_byte_array[2] = { 0 };
	EXPECT_ARRAY_EQ(byte_array, expected_byte_array);
}

TEST(serialize_tests, pack_one_byte_with_most_significant_bit_first) {
	// clang-format off
	bool bit_array[8] = { 0, 0, 0, 0, 1, 1, 1, 1, };
	uint8_t byte_array[1] = { 0 };
	// clang-format on

	Serialize_pack_bits_into_bytes(bit_array, arraylen(bit_array), byte_array, arraylen(byte_array), BIT_ORDERING_MSB_FIRST);

	const uint8_t expected_byte = { 0b11110000 };
	EXPECT_EQ(byte_array[0], expected_byte);
}

TEST(serialize_tests, pack_one_byte_with_least_significant_bit_first) {
	// clang-format off
	bool bit_array[8] = { 0, 0, 0, 0, 1, 1, 1, 1, };
	uint8_t byte_array[1] = { 0 };
	// clang-format on

	Serialize_pack_bits_into_bytes(bit_array, arraylen(bit_array), byte_array, arraylen(byte_array), BIT_ORDERING_LSB_FIRST);

	const uint8_t expected_byte = { 0b00001111 };
	EXPECT_EQ(byte_array[0], expected_byte);
}
