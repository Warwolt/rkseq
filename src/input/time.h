#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>

void Time_timer0_overflow_irq(void);
uint32_t Time_now_ms(void);
uint64_t Time_now_us(void);

#endif /* TIMING_H */
