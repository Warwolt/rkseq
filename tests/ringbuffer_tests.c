#include <rktest/rktest.h>

#define RING_BUFFER_SIZE 16
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
