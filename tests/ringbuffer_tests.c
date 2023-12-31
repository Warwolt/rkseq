#include <rktest/rktest.h>

#define RING_BUFFER_SIZE 4
#include "ringbuffer.h"

TEST(ringbuffer_tests, zero_initialized_buffer_is_empty) {
	ringbuffer_t buffer = { 0 };
	EXPECT_TRUE(ringbuffer_is_empty(&buffer));
}

TEST(ringbuffer_tests, zero_initialized_buffer_is_not_full) {
	ringbuffer_t buffer = { 0 };
	EXPECT_FALSE(ringbuffer_is_full(&buffer));
}

TEST(ringbuffer_tests, reading_after_writing_gives_back_written_byte) {
	ringbuffer_t buffer = { 0 };

	ringbuffer_write(&buffer, 123);
	const uint8_t byte = ringbuffer_read(&buffer);

	EXPECT_EQ(byte, 123);
}

TEST(ringbuffer_tests, writing_makes_buffer_non_empty) {
	ringbuffer_t buffer = { 0 };

	ringbuffer_write(&buffer, 123);

	EXPECT_FALSE(ringbuffer_is_empty(&buffer));
}

TEST(ringbuffer_tests, writing_to_capacity_makes_buffer_full) {
	ringbuffer_t buffer = { 0 };

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		ringbuffer_write(&buffer, i);
	}

	EXPECT_TRUE(ringbuffer_is_full(&buffer));
}

TEST(ringbuffer_tests, all_bytes_written_until_full_can_be_read) {
	ringbuffer_t buffer = { 0 };
	uint8_t output[RING_BUFFER_SIZE] = { 0 };

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		ringbuffer_write(&buffer, i);
	}

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		output[i] = ringbuffer_read(&buffer);
	}

	for (uint8_t i = 0; i < RING_BUFFER_SIZE; i++) {
		ASSERT_EQ_INFO(output[i], i, "i = %d", i);
	}
}

// writing past buffer discards bytes
