#include <stddef.h>
#include <stdint.h>

void hw_serial_rx_complete_irq(void);
void hw_serial_tx_udr_empty_irq(void);

void hw_serial_initialize(int baud);
uint8_t hw_serial_num_available_bytes(void);
void hw_serial_print(const char* str);
void hw_serial_read_string(char* str_buf, size_t str_buf_len);
