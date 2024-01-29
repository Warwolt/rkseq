#ifndef SW_SERIAL_H
#define SW_SERIAL_H

#include "hardware/gpio.h"

#include <stdint.h>

typedef struct {
	int: 0;
} SoftwareSerial;

void SoftwareSerial_pin_change_irq(void);

SoftwareSerial SoftwareSerial_init(uint16_t baud, GpioPin rx_pin, GpioPin tx_pin);
uint16_t SoftwareSerial_available_bytes(SoftwareSerial sw_serial);
void SoftwareSerial_read(SoftwareSerial sw_serial, uint8_t* byte);
void SoftwareSerial_read_bytes(SoftwareSerial sw_serial, uint8_t* byte_buf, uint16_t byte_buf_len);
void SoftwareSerial_write(SoftwareSerial sw_serial, uint8_t byte);

#endif /* SW_SERIAL_H */
