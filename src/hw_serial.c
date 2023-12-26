// based on the Arduino Core `Serial` C++ class

#include "hw_serial.h"

#include "bits.h"
#include "gpio.h"
#include "serial.h"
#include "timer0.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

#define SERIAL_RING_BUFFER_SIZE 64

typedef struct {
	volatile uint8_t head;
	volatile uint8_t tail;
	uint8_t buffer[SERIAL_RING_BUFFER_SIZE];
} ringbuffer_t;

static ringbuffer_t g_rx;
static ringbuffer_t g_tx;

static void rx_complete_irq(void) {
	const uint8_t byte = UDR0;
	const bool parity_error = bit_is_set(UCSR0A, UPE0);
	if (parity_error) {
		return; // Parity error, discard read byte
	}

	uint8_t next_index = (g_rx.head + 1) % SERIAL_RING_BUFFER_SIZE;
	if (next_index == g_rx.tail) {
		return; // About to overflow, discard read byte
	}

	// Write read byte to buffer
	g_rx.buffer[g_rx.head] = byte;
	g_rx.head = next_index;
}

static void tx_udr_empty_irq(void) {
	// If interrupts are enabled, there must be more data in the output
	// buffer. Send the next byte
	unsigned char byte = g_tx.buffer[g_tx.tail];
	g_tx.tail = (g_tx.tail + 1) % SERIAL_RING_BUFFER_SIZE;

	UDR0 = byte;

	if (g_tx.head == g_tx.tail) {
		// Buffer empty, so disable interrupts
		clear_bit(UCSR0B, UDRIE0);
	}
}

ISR(USART_RX_vect) {
	rx_complete_irq();
}

ISR(USART_UDRE_vect) {
	tx_udr_empty_irq();
}

static int hw_serial_read_byte(void) {
	// if the head isn't ahead of the tail, we don't have any characters
	if (g_rx.head == g_rx.tail) {
		return -1;
	}

	uint8_t byte = g_rx.buffer[g_rx.tail];
	g_rx.tail = (g_rx.tail + 1) % SERIAL_RING_BUFFER_SIZE;
	return byte;
}

static int hw_serial_read_byte_with_timeout() {
	const unsigned long timeout_ms = 1000;
	const unsigned long start_ms = timer0_now_ms();
	int byte;
	do {
		byte = hw_serial_read_byte();
		if (byte >= 0) {
			return byte;
		}
	} while (timer0_now_ms() - start_ms < timeout_ms);
	return -1; // timed out
}

static void hw_serial_read_string(char* str_buf, size_t str_buf_len) {
	int index = 0;
	int byte = hw_serial_read_byte_with_timeout();
	while (byte >= 0 && index < str_buf_len) {
		str_buf[index++] = (char)byte;
		byte = hw_serial_read_byte_with_timeout();
	}
}

static void hw_serial_write(uint8_t byte) {
	uint8_t next_index = (g_tx.head + 1) % SERIAL_RING_BUFFER_SIZE;
	g_tx.buffer[g_tx.head] = byte;

	// If the output buffer is full, there's nothing for it other than to
	// wait for the interrupt handler to empty it a bit
	while (next_index == g_tx.tail) {
		if (bit_is_clear(SREG, SREG_I)) {
			// Interrupts are disabled, so we'll have to poll the data
			// register empty flag ourselves. If it is set, pretend an
			// interrupt has happened and call the handler to free up
			// space for us.
			if (bit_is_set(UCSR0A, UDRE0)) {
				tx_udr_empty_irq();
			}
		} else {
			// nop, the interrupt handler will free up space for us
		}
	}

	// Make atomic to prevent execution of ISR between setting the
	// head pointer and setting the interrupt flag resulting in buffer
	// retransmission
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		g_tx.head = next_index;
		set_bit(UCSR0B, UDRIE0);
	}
}

static void hw_serial_print(const char* str) {
	while (*str) {
		hw_serial_write(*str);
		str++;
	}
}

static uint8_t hw_serial_num_available_bytes(void) {
	return (SERIAL_RING_BUFFER_SIZE + g_rx.head - g_rx.tail) % SERIAL_RING_BUFFER_SIZE;
}

serial_t hw_serial_initialize(int baud) {
	// enable "double the USART transmission speed"
	UCSR0A = 1 << U2X0;

	// set the baud rate
	const uint16_t baud_setting = (F_CPU / 4 / baud - 1) / 2;
	UBRR0L = baud_setting;
	UBRR0H = baud_setting >> 8;

	set_bit(UCSR0B, RXEN0); // enable UART Rx
	set_bit(UCSR0B, TXEN0); // enable UART Tx
	set_bit(UCSR0B, RXCIE0); // enable receive interrupts
	clear_bit(UCSR0B, UDRIE0); // disable data register empty interrupts

	return (serial_t) {
		.read_string = &hw_serial_read_string,
		.print = &hw_serial_print,
		.num_available_bytes = &hw_serial_num_available_bytes,
	};
}
