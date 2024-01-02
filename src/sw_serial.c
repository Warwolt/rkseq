#include "sw_serial.h"

#include "bits.h"
#include "gpio.h"
#include "ringbuffer.h"

#include <avr/io.h>
#include <util/delay.h>

#define min(x, y) ((x) < (y) ? (x) : (y))

#define BIT_PERIOD_NS(baud) (1e9 / baud)
#define NS_PER_4_INSTRUCTIONS (4 * 1e9 / F_CPU)
#define BIT_PERIOD_DELAY(baud) (BIT_PERIOD_NS(baud) / NS_PER_4_INSTRUCTIONS) // delay in units of 4 instructions

#define IRQ_DELAY_NS 6920 // measured with oscilloscope, the time from RX signal change to ISR being called
#define IRQ_DELAY 6920 / NS_PER_4_INSTRUCTIONS // delay in units of 4 instructions

#define RX_PIN \
	(gpio_pin_t) { .port = &PORTD, .num = 0 }

static uint16_t g_bit_period_delay;
static ringbuffer_t g_rx_buffer;

void sw_serial_pin_change_irq(void) {
	if (gpio_pin_read(RX_PIN) == 0) {
		uint8_t byte = 0;

		_delay_loop_2(g_bit_period_delay * 1.5 - IRQ_DELAY);
		for (int i = 0; i < 8; i++) {
			const uint8_t bit = gpio_pin_read(RX_PIN);
			byte |= bit << i;

			// subtract 8 to compensate for loop and function calls
			_delay_loop_2(g_bit_period_delay - 8);
		}

		ringbuffer_write(&g_rx_buffer, byte);
	}
}

void sw_serial_initialize(uint16_t baud) {
	g_bit_period_delay = BIT_PERIOD_DELAY(baud);
	gpio_pin_configure(RX_PIN, PIN_MODE_INPUT);
	set_bit(PCICR, PCIE2); // enable pin change interrupts
	set_bit(PCMSK2, PCINT16); // configure PD0-pin (Rx) to trigger interrupts
}

uint16_t sw_serial_available_bytes(void) {
	return ringbuffer_available_bytes(&g_rx_buffer);
}

void sw_serial_read_bytes(uint8_t* byte_buf, uint16_t byte_buf_len) {
	uint16_t bytes_to_read = min(sw_serial_available_bytes, byte_buf_len);
	for (uint16_t i = 0; i < bytes_to_read; i++) {
		ringbuffer_read(&g_rx_buffer, &byte_buf[i]);
	}
}
