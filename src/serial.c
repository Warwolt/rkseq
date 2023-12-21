#include "serial.h"

#include "timer.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

#define clear_bit(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define set_bit(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

void serial_initialize(int baud) {
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
}

#define SERIAL_RING_BUFFER_SIZE 64

typedef struct {
	volatile uint8_t head;
	volatile uint8_t tail;
	uint8_t buffer[SERIAL_RING_BUFFER_SIZE];
} ringbuffer_t;

typedef struct {
	ringbuffer_t rx;
	ringbuffer_t tx;
} serial_t;

static serial_t g_serial = { 0 };

static void rx_complete_irq(void) {
	const uint8_t byte = UDR0;
	const bool parity_error = bit_is_set(UCSR0A, UPE0);
	if (parity_error) {
		return; // Parity error, discard read byte
	}

	uint8_t next_index = (g_serial.rx.head + 1) % SERIAL_RING_BUFFER_SIZE;
	if (next_index == g_serial.rx.tail) {
		return; // About to overflow, discard read byte
	}

	// Write read byte to buffer
	g_serial.rx.buffer[g_serial.rx.head] = byte;
	g_serial.rx.head = next_index;
}

ISR(USART_RX_vect) {
	rx_complete_irq();
}

static void tx_udr_empty_irq(void) {
	// If interrupts are enabled, there must be more data in the output
	// buffer. Send the next byte
	unsigned char byte = g_serial.tx.buffer[g_serial.tx.tail];
	g_serial.tx.tail = (g_serial.tx.tail + 1) % SERIAL_RING_BUFFER_SIZE;

	UDR0 = byte;

	if (g_serial.tx.head == g_serial.tx.tail) {
		// Buffer empty, so disable interrupts
		clear_bit(UCSR0B, UDRIE0);
	}
}

ISR(USART_UDRE_vect) {
	tx_udr_empty_irq();
}

static int serial_read_byte(void) {
	// if the head isn't ahead of the tail, we don't have any characters
	if (g_serial.rx.head == g_serial.rx.tail) {
		return -1;
	}

	uint8_t byte = g_serial.rx.buffer[g_serial.rx.tail];
	g_serial.rx.tail = (g_serial.rx.tail + 1) % SERIAL_RING_BUFFER_SIZE;
	return byte;
}

static int serial_read_byte_with_timeout() {
	const unsigned long timeout_ms = 1000;
	const unsigned long start_ms = timer_now_ms();
	int byte;
	do {
		byte = serial_read_byte();
		if (byte >= 0) {
			return byte;
		}
	} while (timer_now_ms() - start_ms < timeout_ms);
	return -1; // timed out
}

void serial_read_string(char* str_buf, size_t str_buf_len) {
	int index = 0;
	int byte = serial_read_byte_with_timeout();
	while (byte >= 0 && index < str_buf_len) {
		str_buf[index++] = (char)byte;
		byte = serial_read_byte_with_timeout();
	}
}

static void serial_write(uint8_t byte) {
	uint8_t next_index = (g_serial.tx.head + 1) % SERIAL_RING_BUFFER_SIZE;
	g_serial.tx.buffer[g_serial.tx.head] = byte;

	// If the output buffer is full, there's nothing for it other than to
	// wait for the interrupt handler to empty it a bit
	while (next_index == g_serial.tx.tail) {
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
		g_serial.tx.head = next_index;
		set_bit(UCSR0B, UDRIE0);
	}
}

void serial_print(const char* str) {
	while (*str) {
		serial_write(*str);
		str++;
	}
}

static uint8_t serial_num_available_bytes(void) {
	return (SERIAL_RING_BUFFER_SIZE + g_serial.rx.head - g_serial.rx.tail) % SERIAL_RING_BUFFER_SIZE;
}
