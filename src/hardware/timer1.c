#include "hardware/timer1.h"

#include "util/bits.h"

void timer1_initialize(void) {
	set_bit(TIMSK1, OCIE1A); // Enable "Output Compare A Match" interrupts
	set_bit(TCCR1B, WGM12); // Clear counter on "Compare A Match"
}

void timer1_set_period(uint16_t ticks) {
	OCR1A = ticks;
}

void timer1_start(void) {
	// Start timer1 by connecting to CPU clock via prescaler 8
	clear_bit(TCCR1B, CS10);
	set_bit(TCCR1B, CS11);
	clear_bit(TCCR1B, CS12);
}

void timer1_stop(void) {
	// Stop timer1 by disconnecting the CPU clock
	clear_bit(TCCR1B, CS10);
	clear_bit(TCCR1B, CS11);
	clear_bit(TCCR1B, CS12);

	// Reset timer1 counter
	TCNT1 = 0;
}