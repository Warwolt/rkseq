#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stdint.h>

#define RING_BUFFER_DEFAULT_SIZE 64

#ifndef RING_BUFFER_SIZE
#define RING_BUFFER_SIZE RING_BUFFER_DEFAULT_SIZE
#endif

typedef struct {
	uint8_t head;
	uint8_t tail;
	uint8_t size;
	uint8_t buffer[RING_BUFFER_SIZE];
} RingBuffer;

static inline bool RingBuffer_is_empty(const RingBuffer* rb) {
	return rb->size == 0;
}

static inline bool RingBuffer_is_full(const RingBuffer* rb) {
	return rb->size == RING_BUFFER_SIZE;
}

static inline uint16_t RingBuffer_available_bytes(const RingBuffer* rb) {
	return (RING_BUFFER_SIZE + rb->head - rb->tail) % RING_BUFFER_SIZE;
}

static inline int RingBuffer_write(RingBuffer* rb, uint8_t byte) {
	if (RingBuffer_is_full(rb)) {
		return -1;
	}

	rb->buffer[rb->head] = byte;
	rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
	rb->size += 1;
	return 0;
}

static inline int RingBuffer_read(RingBuffer* rb, uint8_t* byte) {
	if (RingBuffer_is_empty(rb)) {
		return -1;
	}
	if (byte) {
		*byte = rb->buffer[rb->tail];
	}
	rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
	rb->size -= 1;
	return 0;
}

#endif /* RING_BUFFER_H */
