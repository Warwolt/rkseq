#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

#define RING_BUFFER_DEFAULT_SIZE 64

#ifndef RING_BUFFER_SIZE
#define RING_BUFFER_SIZE RING_BUFFER_DEFAULT_SIZE
#endif

typedef struct {
	volatile uint8_t head;
	uint8_t size;
	uint8_t buffer[RING_BUFFER_SIZE];
} ringbuffer_t;

static inline bool ringbuffer_is_empty(const ringbuffer_t* rb) {
	return rb->size == 0;
}
// ringbuffer_write
// ringbuffer_read

#undef RING_BUFFER_SIZE

#endif /* RING_BUFFER_H */
