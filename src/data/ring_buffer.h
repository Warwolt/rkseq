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
} ring_buffer_t;

static inline bool ring_buffer_is_empty(const ring_buffer_t* rb) {
	return rb->size == 0;
}

static inline bool ring_buffer_is_full(const ring_buffer_t* rb) {
	return rb->size == RING_BUFFER_SIZE;
}

static inline uint16_t ring_buffer_available_bytes(const ring_buffer_t* rb) {
	return (RING_BUFFER_SIZE + rb->head - rb->tail) % RING_BUFFER_SIZE;
}

static inline int ring_buffer_write(ring_buffer_t* rb, uint8_t byte) {
	if (ring_buffer_is_full(rb)) {
		return -1;
	}

	rb->buffer[rb->head] = byte;
	rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
	rb->size += 1;
	return 0;
}

static inline int ring_buffer_read(ring_buffer_t* rb, uint8_t* byte) {
	if (ring_buffer_is_empty(rb)) {
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
