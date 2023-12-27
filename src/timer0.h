#include <stdint.h>

void timer0_timer_overflow_irq(void);

void timer0_initialize(void);
uint32_t timer0_now_ms(void);
uint64_t timer0_now_us(void);
