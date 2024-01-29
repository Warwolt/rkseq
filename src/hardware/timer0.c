#include "hardware/timer0.h"

#include "util/bits.h"

#include <avr/io.h>

// Configures Timer 0 to be used for counting elapsed milliseconds
Timer0 Timer0_init() {
	// Set prescaler to 8
	set_bit(TCCR0B, CS01);
	clear_bit(TCCR0B, CS00);

	// Enable timer 0 overflow interrupt
	set_bit(TIMSK0, TOIE0);

	return (Timer0) {};
}
