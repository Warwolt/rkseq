#ifndef SW_SERIAL_H
#define SW_SERIAL_H

#include "gpio.h"

#include <stdint.h>

void sw_serial_pin_change_irq(void);

void sw_serial_initialize(uint16_t baud, gpio_pin_t rx_pin);
uint16_t sw_serial_available_bytes(void);
void sw_serial_read_bytes(uint8_t* byte_buf, uint16_t byte_buf_len);

#endif /* SW_SERIAL_H */
