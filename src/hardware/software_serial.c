#include "hardware/software_serial.h"

#include "data/ring_buffer.h"
#include "hardware/gpio.h"
#include "util/bits.h"
#include "util/math.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

static GpioPin g_rx_pin;
static GpioPin g_tx_pin;
static RingBuffer g_rx_buffer;

static uint16_t g_start_bit_delay;
static uint16_t g_data_bit_delay;
static uint16_t g_stop_bit_delay;
static uint16_t g_tx_bit_delay;

void SoftwareSerial_pin_change_irq(void) {
	if (GpioPin_read(g_rx_pin) == 0) { // check if start bit is present
		clear_bit(PCICR, PCIE2); // disable interrupts while receiving
		uint8_t byte = 0;

		_delay_loop_2(g_start_bit_delay);
		for (int i = 0; i < 8; i++) {
			// subtract to compensate for loop and function calls
			_delay_loop_2(g_data_bit_delay - 23);

			const uint8_t bit = GpioPin_read(g_rx_pin);
			byte |= bit << i;
		}

		RingBuffer_write(&g_rx_buffer, byte);

		_delay_loop_2(g_stop_bit_delay);
		set_bit(PCICR, PCIE2); // re-enable pin change interrupts
	}
}

SoftwareSerial SoftwareSerial_init(uint16_t baud, GpioPin rx_pin, GpioPin tx_pin) {
	g_rx_pin = rx_pin;
	g_tx_pin = tx_pin;

	// This borrows the delay compensations from
	// https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/SoftwareSerial/src/SoftwareSerial.cpp#L304
	g_data_bit_delay = (F_CPU / baud) / 4;
	g_start_bit_delay = clamped_subtract(g_data_bit_delay / 2, (4 + 4 + 75 + 17 - 23) / 4);
	g_stop_bit_delay = g_data_bit_delay;
	g_tx_bit_delay = clamped_subtract(g_data_bit_delay, 36 / 4);

	GpioPin_configure(g_rx_pin, PIN_MODE_INPUT);
	GpioPin_configure(g_tx_pin, PIN_MODE_OUTPUT);
	set_bit(PCICR, PCIE2); // enable pin change interrupts

	// FIXME: this should either be deduced from rx_pin arg or be configured elsewhere!!!
	set_bit(PCMSK2, PCINT18); // configure PD2-pin (Rx) to trigger interrupts

	return (SoftwareSerial) {};
}

uint16_t SoftwareSerial_available_bytes(SoftwareSerial sw_serial) {
	return RingBuffer_available_bytes(&g_rx_buffer);
}

void SoftwareSerial_read(SoftwareSerial sw_serial, uint8_t* byte) {
	RingBuffer_read(&g_rx_buffer, byte);
}

void SoftwareSerial_read_bytes(SoftwareSerial sw_serial, uint8_t* byte_buf, uint16_t byte_buf_len) {
	uint16_t bytes_to_read = min(SoftwareSerial_available_bytes(sw_serial), byte_buf_len);
	for (uint16_t i = 0; i < bytes_to_read; i++) {
		RingBuffer_read(&g_rx_buffer, &byte_buf[i]);
	}
}

void SoftwareSerial_write(SoftwareSerial sw_serial, uint8_t byte) {
	// copy into local variables to make compiler put the
	// values into registers before disabling interrupts.
	const GpioPin tx_pin = g_tx_pin;
	const uint16_t bit_delay = g_tx_bit_delay;
	const uint8_t old_SREG = SREG;

	cli(); // disable interrupts to not mess with timing

	// write start bit
	GpioPin_clear(tx_pin);
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
	GpioPin_set(tx_pin);
	_delay_loop_2(bit_delay);

	// re-enable interrupts
	SREG = old_SREG;
	sei();
}
