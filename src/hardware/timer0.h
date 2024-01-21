#ifndef TIMER0_H
#define TIMER0_H

#include <stdint.h>

void Timer0_timer_overflow_irq(void);

void Timer0_initialize(void);
uint32_t Timer0_now_ms(void);
uint64_t Timer0_now_us(void);

#endif /* TIMER0_H */
