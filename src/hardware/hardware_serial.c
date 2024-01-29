// based on the Arduino Core `Serial` C++ class

#include "hardware/hardware_serial.h"

#include "hardware/gpio.h"
#include "input/time.h"
#include "util/bits.h"

#define RING_BUFFER_SIZE 64
#include "data/ring_buffer.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/atomic.h>

static RingBuffer g_rx_buffer;
static RingBuffer g_tx_buffer;
static Timer0* g_timer0;

void HardwareSerial_rx_complete_irq(void) {
	const uint8_t byte = UDR0;
	const bool parity_error = bit_is_set(UCSR0A, UPE0);
	if (parity_error) {
		return; // Parity error, discard read byte
	}

	RingBuffer_write(&g_rx_buffer, byte);
}

void HardwareSerial_tx_udr_empty_irq(void) {
	// If interrupts are enabled, there must be more data in the output
	// buffer. Send the next byte
	RingBuffer_read(&g_tx_buffer, (uint8_t*)&UDR0);

	if (RingBuffer_is_empty(&g_tx_buffer)) {
		// Buffer empty, so disable interrupts
		clear_bit(UCSR0B, UDRIE0);
	}
}

static int HardwareSerial_read_byte_with_timeout() {
	if (!g_timer0) {
		return -2; // not initialized
	}

	const unsigned long timeout_ms = 1000;
	const unsigned long start_ms = Time_now_ms(*g_timer0);
	uint8_t byte;
	do {
		if (RingBuffer_read(&g_rx_buffer, &byte) == 0) {
			return byte;
		}
	} while (Time_now_ms(*g_timer0) - start_ms < timeout_ms);
	return -1; // timed out
}

void HardwareSerial_write(uint8_t byte) {
	RingBuffer_write(&g_tx_buffer, byte);

	// If the output buffer is full, there's nothing for it other than to
	// wait for the interrupt handler to empty it a bit
	while (RingBuffer_is_full(&g_tx_buffer)) {
		if (bit_is_clear(SREG, SREG_I)) {
			// Interrupts are disabled, so we'll have to poll the data
			// register empty flag ourselves. If it is set, pretend an
			// interrupt has happened and call the handler to free up
			// space for us.
			if (bit_is_set(UCSR0A, UDRE0)) {
				HardwareSerial_tx_udr_empty_irq();
			}
		} else {
			// nop, the interrupt handler will free up space for us
		}
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		set_bit(UCSR0B, UDRIE0);
	}
}

void HardwareSerial_read_buf(uint8_t* buf, size_t buf_len) {
	size_t index = 0;
	int byte = HardwareSerial_read_byte_with_timeout();
	while (byte >= 0 && index < buf_len) {
		buf[index++] = (uint8_t)byte;
		byte = HardwareSerial_read_byte_with_timeout();
	}
}

uint8_t HardwareSerial_num_available_bytes(void) {
	return (RING_BUFFER_SIZE + g_rx_buffer.head - g_rx_buffer.tail) % RING_BUFFER_SIZE;
}

void HardwareSerial_init(int baud) {
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
