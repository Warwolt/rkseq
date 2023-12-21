#include <stddef.h>
#include <stdint.h>

typedef struct {
    void (*read_string)(char* str_buf, size_t str_buf_len);
    void (*print)(const char* str);
    uint8_t (*num_available_bytes)(void);
} iserial_t;

