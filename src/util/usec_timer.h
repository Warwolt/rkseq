#ifndef USEC_TIMER_H
#define USEC_TIMER_H

#include "hardware/gpio.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint64_t start_time_us;
	uint64_t period_us;
} usec_timer_t;

usec_timer_t usec_timer_init(uint64_t period_us);
void usec_timer_reset(usec_timer_t* timer);
bool usec_timer_period_has_elapsed(const usec_timer_t* timer);

#endif /* USEC_TIMER_H */
