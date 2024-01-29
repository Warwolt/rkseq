#include "util/timer.h"

#include "input/time.h"

MicrosecondTimer MicrosecondTimer_init(Timer0 timer0, uint64_t period_us) {
	return (MicrosecondTimer) {
		.timer0 = timer0,
		.start_time_us = Time_now_us(timer0),
		.period_us = period_us,
	};
}

void MicrosecondTimer_reset(MicrosecondTimer* timer) {
	timer->start_time_us = Time_now_us(timer->timer0);
}

bool MicrosecondTimer_elapsed(const MicrosecondTimer* timer) {
	return (Time_now_us(timer->timer0) - timer->start_time_us >= timer->period_us);
}

MillisecondTimer MillisecondTimer_init(Timer0 timer0, uint32_t period_ms) {
	return (MillisecondTimer) {
		.timer0 = timer0,
		.start_time_ms = Time_now_ms(timer0),
		.period_ms = period_ms,
	};
}

void MillisecondTimer_reset(MillisecondTimer* timer) {
	timer->start_time_ms = Time_now_ms(timer->timer0);
}

bool MillisecondTimer_elapsed(const MillisecondTimer* timer) {
	return (Time_now_ms(timer->timer0) - timer->start_time_ms >= timer->period_ms);
}
