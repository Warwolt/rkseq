#include <stddef.h>
#include <stdint.h>

void serial_initialize(int baud);
void serial_read_string(char* str_buf, size_t str_buf_len);
void serial_print(const char* str);
uint8_t serial_num_available_bytes(void);
