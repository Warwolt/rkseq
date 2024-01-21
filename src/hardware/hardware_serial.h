#ifndef HW_SERIAL_H
#define HW_SERIAL_H

#include <stddef.h>
#include <stdint.h>

void HardwareSerial_rx_complete_irq(void);
void HardwareSerial_tx_udr_empty_irq(void);

void HardwareSerial_init(int baud);
uint8_t HardwareSerial_num_available_bytes(void);
void HardwareSerial_write(uint8_t byte);
void HardwareSerial_read_buf(uint8_t* buf, size_t buf_len);

#endif /* HW_SERIAL_H */
