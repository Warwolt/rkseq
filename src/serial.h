#include <stddef.h>

void serial_initialize(int baud);
void serial_read_string(char* str_buf, size_t str_buf_len);
void serial_print(const char* str);
