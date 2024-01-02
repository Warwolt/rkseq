#ifndef HW_SERIAL_H
#define HW_SERIAL_H

#include <stddef.h>
#include <stdint.h>

void hw_serial_rx_complete_irq(void);
void hw_serial_tx_udr_empty_irq(void);

void hw_serial_initialize(int baud);
uint8_t hw_serial_num_available_bytes(void);
void hw_serial_write(uint8_t byte);
void hw_serial_read_buf(uint8_t* buf, size_t buf_len);

#endif /* HW_SERIAL_H */
