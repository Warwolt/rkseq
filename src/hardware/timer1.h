#ifndef TIMER_1_H
#define TIMER_1_H

#include <stdint.h>

#define TIMER1_USEC_PER_TICK 0.5f

typedef struct {
	int: 0;
} Timer1;

Timer1 Timer1_init(void);
void Timer1_set_period(Timer1 timer1, uint16_t ticks);
void Timer1_start(Timer1 timer1);
void Timer1_stop(Timer1 timer1);

#endif /* TIMER_1_H */
