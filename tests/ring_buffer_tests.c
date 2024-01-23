#include <rktest/rktest.h>

#define RING_BUFFER_SIZE 4
#include "data/ring_buffer.h"

TEST(RingBuffer, zero_initialized_buffer_is_empty) {
	RingBuffer buffer = { 0 };
	EXPECT_TRUE(RingBuffer_is_empty(&buffer));
}

TEST(RingBuffer, zero_initialized_buffer_is_not_full) {
	RingBuffer buffer = { 0 };
	EXPECT_FALSE(RingBuffer_is_full(&buffer));
}

TEST(RingBuffer, reading_after_writing_gives_back_written_byte) {
	RingBuffer buffer = { 0 };

	RingBuffer_write(&buffer, 123);
	uint8_t byte;
	RingBuffer_read(&buffer, &byte);

	EXPECT_EQ(byte, 123);
}

TEST(RingBuffer, writing_makes_buffer_non_empty) {
	RingBuffer buffer = { 0 };

	RingBuffer_write(&buffer, 123);

	EXPECT_FALSE(RingBuffer_is_empty(&buffer));
}

TEST(RingBuffer, writing_to_capacity_makes_buffer_full) {
	RingBuffer buffer = { 0 };

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		RingBuffer_write(&buffer, i);
	}

	EXPECT_TRUE(RingBuffer_is_full(&buffer));
}

TEST(RingBuffer, all_bytes_written_until_full_can_be_read) {
	RingBuffer buffer = { 0 };
	uint8_t output[RING_BUFFER_SIZE] = { 0 };

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		RingBuffer_write(&buffer, i);
	}

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		RingBuffer_read(&buffer, &output[i]);
	}

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		ASSERT_EQ_INFO(output[i], i, "i = %d", i);
	}
}

TEST(RingBuffer, writing_past_buffer_discards_bytes) {
	RingBuffer buffer = { 0 };

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		RingBuffer_write(&buffer, i);
	}
	RingBuffer_write(&buffer, 123); // byte should be discarded

	uint8_t byte;
	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		RingBuffer_read(&buffer, &byte);
		ASSERT_EQ(byte, i);
	}
	RingBuffer_read(&buffer, &byte); // byte should not be updated

	EXPECT_EQ_INFO(byte, RING_BUFFER_SIZE - 1, "Expected last write to have been discarded");
}

TEST(RingBuffer, reading_from_empty_buffer_gives_nothing) {
	RingBuffer buffer = { 0 };

	uint8_t byte = 123;
	RingBuffer_read(&buffer, &byte);

	EXPECT_EQ_INFO(byte, 123, "Byte should not have been written to");
}

TEST(RingBuffer, writing_increases_available_bytes) {
	RingBuffer buffer = { 0 };

	RingBuffer_write(&buffer, 11);
	RingBuffer_write(&buffer, 22);
	RingBuffer_write(&buffer, 33);
	uint16_t available_bytes = RingBuffer_available_bytes(&buffer);

	EXPECT_EQ(available_bytes, 3);
}

TEST(RingBuffer, reading_decreases_available_bytes) {
	RingBuffer buffer = { 0 };

	uint8_t byte;
	RingBuffer_write(&buffer, 11);
	RingBuffer_write(&buffer, 22);
	RingBuffer_write(&buffer, 33);
	RingBuffer_write(&buffer, 44);
	RingBuffer_read(&buffer, NULL);
	RingBuffer_read(&buffer, NULL);
	uint16_t available_bytes = RingBuffer_available_bytes(&buffer);

	EXPECT_EQ(available_bytes, 2);
}
