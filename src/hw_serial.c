// based on the Arduino Core `Serial` C++ class

#include "hw_serial.h"

#include "bits.h"
#include "gpio.h"
#include "timer0.h"

#define RING_BUFFER_SIZE 64
#include "ringbuffer.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

static ringbuffer_t g_rx_buffer;
static ringbuffer_t g_tx_buffer;

void hw_serial_rx_complete_irq(void) {
	const uint8_t byte = UDR0;
	const bool parity_error = bit_is_set(UCSR0A, UPE0);
	if (parity_error) {
		return; // Parity error, discard read byte
	}

	ringbuffer_write(&g_rx_buffer, byte);
}

void hw_serial_tx_udr_empty_irq(void) {
	// If interrupts are enabled, there must be more data in the output
	// buffer. Send the next byte
	ringbuffer_read(&g_tx_buffer, (uint8_t*)&UDR0);

	if (ringbuffer_is_empty(&g_tx_buffer)) {
		// Buffer empty, so disable interrupts
		clear_bit(UCSR0B, UDRIE0);
	}
}

static int hw_serial_read_byte_with_timeout() {
	const unsigned long timeout_ms = 1000;
	const unsigned long start_ms = timer0_now_ms();
	uint8_t byte;
	do {
		if (ringbuffer_read(&g_rx_buffer, &byte) == 0) {
			return byte;
		}
	} while (timer0_now_ms() - start_ms < timeout_ms);
	return -1; // timed out
}

void hw_serial_write(uint8_t byte) {
	ringbuffer_write(&g_tx_buffer, byte);

	// If the output buffer is full, there's nothing for it other than to
	// wait for the interrupt handler to empty it a bit
	while (ringbuffer_is_full(&g_tx_buffer)) {
		if (bit_is_clear(SREG, SREG_I)) {
			// Interrupts are disabled, so we'll have to poll the data
			// register empty flag ourselves. If it is set, pretend an
			// interrupt has happened and call the handler to free up
			// space for us.
			if (bit_is_set(UCSR0A, UDRE0)) {
				hw_serial_tx_udr_empty_irq();
			}
		} else {
			// nop, the interrupt handler will free up space for us
		}
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		set_bit(UCSR0B, UDRIE0);
	}
}

void hw_serial_read_buf(uint8_t* buf, size_t buf_len) {
	size_t index = 0;
	int byte = hw_serial_read_byte_with_timeout();
	while (byte >= 0 && index < buf_len) {
		buf[index++] = (uint8_t)byte;
		byte = hw_serial_read_byte_with_timeout();
	}
}

uint8_t hw_serial_num_available_bytes(void) {
	return (RING_BUFFER_SIZE + g_rx_buffer.head - g_rx_buffer.tail) % RING_BUFFER_SIZE;
}

void hw_serial_initialize(int baud) {
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
