#ifndef TIMER_1_H
#define TIMER_1_H

#include <stdint.h>

void timer1_compare_a_match_irq(void);

void timer1_initialize(void);
void timer1_set_compare_a_match_handler(void (*handler)(void*), void* context);
void timer1_set_period(uint16_t ticks);
void timer1_start(void);
void timer1_stop(void);

#endif /* TIMER_1_H */
