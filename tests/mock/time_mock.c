#include "input/time.h"

static uint32_t g_now_ms = 0;
static uint64_t g_now_us = 0;

void MockTime_set_now_ms(uint32_t now_ms) {
	g_now_ms = now_ms;
}
void MockTime_set_now_us(uint32_t now_us) {
	g_now_us = now_us;
}

void Time_timer0_overflow_irq(void) {
}

uint32_t Time_now_ms(void) {
	return g_now_ms;
}

uint64_t Time_now_us(void) {
	return g_now_us;
}
