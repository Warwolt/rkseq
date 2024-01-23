#ifndef TIME_MOCK
#define TIME_MOCK

#include "input/time.h"

#include <stdint.h>

void MockTime_set_now_ms(uint32_t now_ms);
void MockTime_set_now_us(uint64_t now_us);

#endif /* TIME_MOCK */
