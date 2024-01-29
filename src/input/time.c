#include "input/time.h"

#include "hardware/timer0.h"
#include "util/bits.h"

#include <avr/io.h>
#include <util/atomic.h>

#define TIMER0_FREQ (F_CPU / TIMER0_PRESCALER)
#define TICKS_PER_MICROSECOND (TIMER0_FREQ / 1000000L)
#define TICKS_TO_MICROSECONDS(ticks) ((ticks) / TICKS_PER_MICROSECOND)
#define MICROSECONDS_PER_TIMER0_OVERFLOW (TICKS_TO_MICROSECONDS(256))

static volatile uint16_t g_us = 0;
static volatile uint32_t g_ms = 0;

void Time_on_timer0_overflow(Timer0 timer0) {
	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	uint16_t us = g_us;
	uint32_t ms = g_ms;

	us += MICROSECONDS_PER_TIMER0_OVERFLOW;
	if (us >= 1000) {
		us -= 1000;
		ms += 1;
	}

	g_us = us;
	g_ms = ms;
}

// Number of elapsed milliseconds since program start
uint32_t Time_now_ms(Timer0 timer0) {
	uint32_t now_ms;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		now_ms = g_ms;
	}

	return now_ms;
}

// Number of elapsed microseconds since program start
uint64_t Time_now_us(Timer0 timer0) {
	uint64_t now_us;

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
		now_us = TICKS_TO_MICROSECONDS(TCNT0) + g_us + g_ms * 1000;
	}

	return now_us;
}
