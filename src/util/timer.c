#include "util/timer.h"

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

MillisecondTimer MillisecondTimer_init(uint32_t period_ms) {
	return (MillisecondTimer) {
		.start_time_ms = Time_now_ms(),
		.period_ms = period_ms,
	};
}

void MillisecondTimer_reset(MillisecondTimer* timer) {
	timer->start_time_ms = Time_now_ms();
}

bool MillisecondTimer_period_has_elapsed(const MillisecondTimer* timer) {
	return (Time_now_ms() - timer->start_time_ms >= timer->period_ms);
}
