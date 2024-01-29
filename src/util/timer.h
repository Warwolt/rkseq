#ifndef TIMER_H
#define TIMER_H

#include "hardware/timer0.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	Timer0 timer0;
	uint64_t start_time_us;
	uint64_t period_us;
} MicrosecondTimer;

typedef struct {
	Timer0 timer0;
	uint32_t start_time_ms;
	uint32_t period_ms;
} MillisecondTimer;

MicrosecondTimer MicrosecondTimer_init(Timer0 timer0, uint64_t period_us);
void MicrosecondTimer_reset(MicrosecondTimer* timer);
bool MicrosecondTimer_elapsed(const MicrosecondTimer* timer);

MillisecondTimer MillisecondTimer_init(Timer0 timer0, uint32_t period_ms);
void MillisecondTimer_reset(MillisecondTimer* timer);
bool MillisecondTimer_elapsed(const MillisecondTimer* timer);

#endif /* TIMER_H */
