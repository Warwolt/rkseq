#ifndef TIMER0_H
#define TIMER0_H

#include <stdint.h>

#define TIMER0_PRESCALER 8

typedef struct {
	int: 0;
} Timer0;

Timer0 Timer0_init(void);

#endif /* TIMER0_H */
