#ifndef TIMER_1_H
#define TIMER_1_H

#include <stdint.h>

void Timer1_init(void);
void Timer1_set_period(uint16_t ticks);
void Timer1_start(void);
void Timer1_stop(void);

#endif /* TIMER_1_H */
