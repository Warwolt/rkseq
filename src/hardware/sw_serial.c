#include "hardware/sw_serial.h"

#include "data/ring_buffer.h"
#include "hardware/gpio.h"
#include "util/bits.h"
#include "util/math.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

static gpio_pin_t g_rx_pin;
static gpio_pin_t g_tx_pin;
static ring_buffer_t g_rx_buffer;

static uint16_t g_start_bit_delay;
static uint16_t g_data_bit_delay;
static uint16_t g_stop_bit_delay;
static uint16_t g_tx_bit_delay;

void sw_serial_pin_change_irq(void) {
	if (gpio_pin_read(g_rx_pin) == 0) { // check if start bit is present
		clear_bit(PCICR, PCIE2); // disable interrupts while receiving
		uint8_t byte = 0;

		_delay_loop_2(g_start_bit_delay);
		for (int i = 0; i < 8; i++) {
			// subtract to compensate for loop and function calls
			_delay_loop_2(g_data_bit_delay - 23);

			const uint8_t bit = gpio_pin_read(g_rx_pin);
			byte |= bit << i;
			// debug toggle
			{
				toggle_bit(PORTB, 5);
				toggle_bit(PORTB, 5);
			}
		}

		ring_buffer_write(&g_rx_buffer, byte);

		_delay_loop_2(g_stop_bit_delay);
		set_bit(PCICR, PCIE2); // re-enable pin change interrupts
	}
}

void sw_serial_initialize(uint16_t baud, gpio_pin_t rx_pin, gpio_pin_t tx_pin) {
	g_rx_pin = rx_pin;
	g_tx_pin = tx_pin;

	// This borrows the delay compensations from
	// https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/SoftwareSerial/src/SoftwareSerial.cpp#L304
	g_data_bit_delay = (F_CPU / baud) / 4;
	g_start_bit_delay = clamped_subtract(g_data_bit_delay / 2, (4 + 4 + 75 + 17 - 23) / 4);
	g_stop_bit_delay = g_data_bit_delay;
	g_tx_bit_delay = clamped_subtract(g_data_bit_delay, 36 / 4);

	gpio_pin_configure(g_rx_pin, PIN_MODE_INPUT);
	gpio_pin_configure(g_tx_pin, PIN_MODE_OUTPUT);
	set_bit(PCICR, PCIE2); // enable pin change interrupts
	set_bit(PCMSK2, PCINT16); // configure PD0-pin (Rx) to trigger interrupts
}

uint16_t sw_serial_available_bytes(void) {
	return ring_buffer_available_bytes(&g_rx_buffer);
}

void sw_serial_read(uint8_t* byte) {
	ring_buffer_read(&g_rx_buffer, byte);
}

void sw_serial_read_bytes(uint8_t* byte_buf, uint16_t byte_buf_len) {
	uint16_t bytes_to_read = min(sw_serial_available_bytes(), byte_buf_len);
	for (uint16_t i = 0; i < bytes_to_read; i++) {
		ring_buffer_read(&g_rx_buffer, &byte_buf[i]);
	}
}

void sw_serial_write(uint8_t byte) {
	// copy into local variables to make compiler put the
	// values into registers before disabling interrupts.
	const gpio_pin_t tx_pin = g_tx_pin;
	const uint16_t bit_delay = g_tx_bit_delay;
	const uint8_t old_SREG = SREG;

	cli(); // disable interrupts to not mess with timing

	// write start bit
	gpio_pin_clear(tx_pin);
	_delay_loop_2(bit_delay);

	// write bits
	for (uint8_t i = 0; i < 8; i++) {
		const uint8_t bit = (byte >> i) & 0x1;
		if (bit) {
			set_bit(*tx_pin.port, tx_pin.num);
		} else {
			clear_bit(*tx_pin.port, tx_pin.num);
		}
		_delay_loop_2(bit_delay);
	}

	// write stop bit
	gpio_pin_set(tx_pin);
	_delay_loop_2(bit_delay);

	// re-enable interrupts
	SREG = old_SREG;
	sei();
}
