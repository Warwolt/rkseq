#ifndef SW_SERIAL_H
#define SW_SERIAL_H

#include "hardware/gpio.h"

#include <stdint.h>

void sw_serial_pin_change_irq(void);

void sw_serial_initialize(uint16_t baud, GpioPin rx_pin, GpioPin tx_pin);
uint16_t sw_serial_available_bytes(void);
void sw_serial_read(uint8_t* byte);
void sw_serial_read_bytes(uint8_t* byte_buf, uint16_t byte_buf_len);
void sw_serial_write(uint8_t byte);

#endif /* SW_SERIAL_H */
