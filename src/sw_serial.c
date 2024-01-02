#include "sw_serial.h"

#include "bits.h"
#include "gpio.h"
#include "ringbuffer.h"

#include <avr/io.h>
#include <util/delay.h>

#define RX_PIN \
	(gpio_pin_t) { .port = &PORTD, .num = 0 }

static uint16_t g_baud;
static ringbuffer_t g_rx_buffer;

void sw_serial_pin_change_irq(void) {
}

void sw_serial_initialize(uint16_t baud) {
	g_baud = baud;
	gpio_pin_configure(RX_PIN, PIN_MODE_INPUT);
	set_bit(PCICR, PCIE2); // enable pin change interrupts
	set_bit(PCMSK2, PCINT16); // configure PD0-pin (Rx) to trigger interrupts
}
