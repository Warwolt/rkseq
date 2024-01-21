#include "util/usec_timer.h"

#include "hardware/timer0.h"

usec_timer_t usec_timer_init(uint64_t period_us) {
	return (usec_timer_t) {
		.start_time_us = Timer0_now_us(),
		.period_us = period_us,
	};
}

void usec_timer_reset(usec_timer_t* timer) {
	timer->start_time_us = Timer0_now_us();
}

bool usec_timer_period_has_elapsed(const usec_timer_t* timer) {
	return (Timer0_now_us() - timer->start_time_us >= timer->period_us);
}
