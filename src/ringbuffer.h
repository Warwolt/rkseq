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
} ringbuffer_t;

static inline bool ringbuffer_is_empty(const ringbuffer_t* rb) {
	return rb->size == 0;
}

static inline bool ringbuffer_is_full(const ringbuffer_t* rb) {
	return rb->size == RING_BUFFER_SIZE;
}

static inline int ringbuffer_write(ringbuffer_t* rb, uint8_t byte) {
	if (ringbuffer_is_full(rb)) {
		return -1;
	}

	rb->buffer[rb->head] = byte;
	rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
	rb->size += 1;
	return 0;
}

static inline int ringbuffer_read(ringbuffer_t* rb, uint8_t* byte) {
	if (ringbuffer_is_empty(rb)) {
		return -1;
	}
	*byte = rb->buffer[rb->tail];
	rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
	rb->size -= 1;
	return 0;
}

#endif /* RING_BUFFER_H */
