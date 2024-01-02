#ifndef SW_SERIAL_H
#define SW_SERIAL_H

#include <stdint.h>

void sw_serial_pin_change_irq(void);

void sw_serial_initialize(uint16_t baud);

#endif /* SW_SERIAL_H */
