#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>

#include "hardware/timer0.h"

void Time_on_timer0_overflow(Timer0 timer0);
uint32_t Time_now_ms(Timer0 timer0);
uint64_t Time_now_us(Timer0 timer0);

#endif /* TIMING_H */
