#include <rktest/rktest.h>

#define RING_BUFFER_SIZE 4
#include "data/ring_buffer.h"

TEST(ring_buffer_tests, zero_initialized_buffer_is_empty) {
	ring_buffer_t buffer = { 0 };
	EXPECT_TRUE(ring_buffer_is_empty(&buffer));
}

TEST(ring_buffer_tests, zero_initialized_buffer_is_not_full) {
	ring_buffer_t buffer = { 0 };
	EXPECT_FALSE(ring_buffer_is_full(&buffer));
}

TEST(ring_buffer_tests, reading_after_writing_gives_back_written_byte) {
	ring_buffer_t buffer = { 0 };

	ring_buffer_write(&buffer, 123);
	uint8_t byte;
	ring_buffer_read(&buffer, &byte);

	EXPECT_EQ(byte, 123);
}

TEST(ring_buffer_tests, writing_makes_buffer_non_empty) {
	ring_buffer_t buffer = { 0 };

	ring_buffer_write(&buffer, 123);

	EXPECT_FALSE(ring_buffer_is_empty(&buffer));
}

TEST(ring_buffer_tests, writing_to_capacity_makes_buffer_full) {
	ring_buffer_t buffer = { 0 };

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		ring_buffer_write(&buffer, i);
	}

	EXPECT_TRUE(ring_buffer_is_full(&buffer));
}

TEST(ring_buffer_tests, all_bytes_written_until_full_can_be_read) {
	ring_buffer_t buffer = { 0 };
	uint8_t output[RING_BUFFER_SIZE] = { 0 };

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		ring_buffer_write(&buffer, i);
	}

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		ring_buffer_read(&buffer, &output[i]);
	}

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		ASSERT_EQ_INFO(output[i], i, "i = %d", i);
	}
}

TEST(ring_buffer_tests, writing_past_buffer_discards_bytes) {
	ring_buffer_t buffer = { 0 };

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		ring_buffer_write(&buffer, i);
	}
	ring_buffer_write(&buffer, 123); // byte should be discarded

	uint8_t byte;
	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		ring_buffer_read(&buffer, &byte);
		ASSERT_EQ(byte, i);
	}
	ring_buffer_read(&buffer, &byte); // byte should not be updated

	EXPECT_EQ_INFO(byte, RING_BUFFER_SIZE - 1, "Expected last write to have been discarded");
}

TEST(ring_buffer_tests, reading_from_empty_buffer_gives_nothing) {
	ring_buffer_t buffer = { 0 };

	uint8_t byte = 123;
	ring_buffer_read(&buffer, &byte);

	EXPECT_EQ_INFO(byte, 123, "Byte should not have been written to");
}

TEST(ring_buffer_tests, writing_increases_available_bytes) {
	ring_buffer_t buffer = { 0 };

	ring_buffer_write(&buffer, 11);
	ring_buffer_write(&buffer, 22);
	ring_buffer_write(&buffer, 33);
	uint16_t available_bytes = ring_buffer_available_bytes(&buffer);

	EXPECT_EQ(available_bytes, 3);
}

TEST(ring_buffer_tests, reading_decreases_available_bytes) {
	ring_buffer_t buffer = { 0 };

	uint8_t byte;
	ring_buffer_write(&buffer, 11);
	ring_buffer_write(&buffer, 22);
	ring_buffer_write(&buffer, 33);
	ring_buffer_write(&buffer, 44);
	ring_buffer_read(&buffer, NULL);
	ring_buffer_read(&buffer, NULL);
	uint16_t available_bytes = ring_buffer_available_bytes(&buffer);

	EXPECT_EQ(available_bytes, 2);
}
