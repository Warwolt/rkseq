#include <rktest/rktest.h>

#define RING_BUFFER_SIZE 16
#include "ringbuffer.h"

TEST(ringbuffer_tests, zero_initialized_buffer_is_empty) {
	ringbuffer_t buffer = { 0 };
	EXPECT_TRUE(ringbuffer_is_empty(&buffer));
}
