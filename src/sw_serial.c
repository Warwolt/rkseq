#include "sw_serial.h"

#include "bits.h"
#include "gpio.h"
#include "ringbuffer.h"
#include "util_math.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

static gpio_pin_t g_rx_pin;
static ringbuffer_t g_rx_buffer;

static uint16_t g_start_bit_delay;
static uint16_t g_data_bit_delay;
static uint16_t g_stop_bit_delay;

void sw_serial_pin_change_irq(void) {
	if (gpio_pin_read(g_rx_pin) == 0) {
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

		ringbuffer_write(&g_rx_buffer, byte);

		_delay_loop_2(g_stop_bit_delay);
		set_bit(PCICR, PCIE2); // re-enable pin change interrupts
	}
}

// This borrows the delay compensations from
// https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/SoftwareSerial/src/SoftwareSerial.cpp#L304
void sw_serial_initialize(uint16_t baud, gpio_pin_t rx_pin) {
	g_rx_pin = rx_pin;

	g_data_bit_delay = (F_CPU / baud) / 4;
	g_start_bit_delay = clamped_subtract(g_data_bit_delay / 2, (4 + 4 + 75 + 17 - 23) / 4);
	g_stop_bit_delay = g_data_bit_delay;

	gpio_pin_configure(g_rx_pin, PIN_MODE_INPUT);
	set_bit(PCICR, PCIE2); // enable pin change interrupts
	set_bit(PCMSK2, PCINT16); // configure PD0-pin (Rx) to trigger interrupts
}

uint16_t sw_serial_available_bytes(void) {
	return ringbuffer_available_bytes(&g_rx_buffer);
}

void sw_serial_read_bytes(uint8_t* byte_buf, uint16_t byte_buf_len) {
	uint16_t bytes_to_read = min(sw_serial_available_bytes(), byte_buf_len);
	for (uint16_t i = 0; i < bytes_to_read; i++) {
		ringbuffer_read(&g_rx_buffer, &byte_buf[i]);
	}
}
