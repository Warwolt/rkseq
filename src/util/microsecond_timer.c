#include "util/microsecond_timer.h"

#include "input/time.h"

MicrosecondTimer MicrosecondTimer_init(uint64_t period_us) {
	return (MicrosecondTimer) {
		.start_time_us = Time_now_us(),
		.period_us = period_us,
	};
}

void MicrosecondTimer_reset(MicrosecondTimer* timer) {
	timer->start_time_us = Time_now_us();
}

bool MicrosecondTimer_period_has_elapsed(const MicrosecondTimer* timer) {
	return (Time_now_us() - timer->start_time_us >= timer->period_us);
}
