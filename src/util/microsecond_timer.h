#ifndef USEC_TIMER_H
#define USEC_TIMER_H

#include "hardware/gpio.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint64_t start_time_us;
	uint64_t period_us;
} MicrosecondTimer;

MicrosecondTimer MicrosecondTimer_init(uint64_t period_us);
void MicrosecondTimer_reset(MicrosecondTimer* timer);
bool MicrosecondTimer_period_has_elapsed(const MicrosecondTimer* timer);

#endif /* USEC_TIMER_H */
