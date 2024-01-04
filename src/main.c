#include "data/ring_buffer.h"
#include "hardware/gpio.h"
#include "hardware/hw_serial.h"
#include "hardware/rotary_encoder.h"
#include "hardware/sw_serial.h"
#include "hardware/timer0.h"
#include "logging.h"
#include "util/bits.h"
#include "util/math.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

static int8_t digit_segments[10] = {
	0b00111111, // 0
	0b00000110, // 1
	0b01011011, // 2
	0b01001111, // 3
	0b01100110, // 4
	0b01101101, // 5
	0b01111101, // 6
	0b00000111, // 7
	0b01111111, // 8
	0b01100111, // 9
};

typedef struct {
	gpio_pin_t clock_pin;
	gpio_pin_t latch_pin;
	gpio_pin_t data_pin;
	uint8_t digits[4];
	uint8_t current_digit;
} segment_display_t;

segment_display_t segment_display_init(gpio_pin_t clock_pin, gpio_pin_t latch_pin, gpio_pin_t data_pin) {
	gpio_pin_configure(clock_pin, PIN_MODE_OUTPUT);
	gpio_pin_configure(latch_pin, PIN_MODE_OUTPUT);
	gpio_pin_configure(data_pin, PIN_MODE_OUTPUT);
	return (segment_display_t) {
		.clock_pin = clock_pin,
		.latch_pin = latch_pin,
		.data_pin = data_pin,
		.digits = { 0 },
		.current_digit = 0,
	};
}

void segment_display_set_number(segment_display_t* display, uint16_t number) {
	display->digits[0] = number / 1 % 10;
	display->digits[1] = number / 10 % 10;
	display->digits[2] = number / 100 % 10;
	display->digits[3] = number / 1000 % 10;
}

void segment_display_update(segment_display_t* display) {
	// write segments
	int8_t byte = 0;
	if (display->current_digit < 4) {
		byte = ~digit_segments[display->digits[display->current_digit]];
	}
	for (uint8_t i = 0; i < 8; i++) {
		const uint8_t bit = (byte >> ((8 - 1) - i)) & 1;
		gpio_pin_write(display->data_pin, bit);
		gpio_pin_set(display->clock_pin);
		gpio_pin_clear(display->clock_pin);
	}
	// select digit
	for (uint8_t i = 0; i < 8; i++) {
		const uint8_t bit = ((0x1 << display->current_digit) >> ((8 - 1) - i)) & 1;
		gpio_pin_write(display->data_pin, bit);
		gpio_pin_set(display->clock_pin);
		gpio_pin_clear(display->clock_pin);
	}
	// output digit
	gpio_pin_set(display->latch_pin);
	gpio_pin_clear(display->latch_pin);

	display->current_digit = (display->current_digit + 1) % 16;
}

/* ----------------------- Interrupt service routines ----------------------- */
ISR(TIMER0_OVF_vect) {
	timer0_timer_overflow_irq();
}

ISR(PCINT2_vect) {
	sw_serial_pin_change_irq();
}

ISR(USART_RX_vect) {
	hw_serial_rx_complete_irq();
}

ISR(USART_UDRE_vect) {
	hw_serial_tx_udr_empty_irq();
}

/* ------------------------------ Main Program ------------------------------ */
void globally_enable_interrupts(void) {
	sei();
}

int main(void) {
	const gpio_pin_t led_pin = gpio_pin_init(&PORTB, 5);
	const gpio_pin_t midi_rx_pin = gpio_pin_init(&PORTD, 2);
	const gpio_pin_t midi_tx_pin = gpio_pin_init(&PORTD, 3);
	const gpio_pin_t tempo_knob_a_pin = gpio_pin_init(&PORTD, 4);
	const gpio_pin_t tempo_knob_b_pin = gpio_pin_init(&PORTD, 5);
	const gpio_pin_t display_clock_pin = gpio_pin_init(&PORTD, 6);
	const gpio_pin_t display_latch_pin = gpio_pin_init(&PORTD, 7);
	const gpio_pin_t display_data_pin = gpio_pin_init(&PORTB, 0);
	gpio_pin_configure(led_pin, PIN_MODE_OUTPUT);

	globally_enable_interrupts();
	timer0_initialize();
	hw_serial_initialize(9600); // uses PD0 and PD1 for logging
	sw_serial_initialize(31250, midi_rx_pin, midi_tx_pin);

	rotary_encoder_t tempo_knob = rotary_encoder_init(tempo_knob_a_pin, tempo_knob_b_pin);
	segment_display_t tempo_display = segment_display_init(display_clock_pin, display_latch_pin, display_data_pin);

	LOG_INFO("Program Start\n");
	uint32_t last_tick = timer0_now_ms();
	uint8_t tempo_bpm = 120;
	while (true) {
		uint32_t now = timer0_now_ms();
		if (now - last_tick >= 1000) {
			last_tick = now;
			gpio_pin_set(led_pin);
			gpio_pin_clear(led_pin);
		}

		int rotary_diff = rotary_encoder_read(&tempo_knob);
		tempo_bpm = max(tempo_bpm + rotary_diff, 1);
		segment_display_set_number(&tempo_display, tempo_bpm);
		if (rotary_diff != 0) {
			LOG_INFO("%d\n", tempo_bpm);
		}

		// test segment display
		segment_display_update(&tempo_display);
	}
}
