#include "logging.h"

#include "hardware/hw_serial.h"
#include "hardware/timer0.h"
#include "util/bits.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/atomic.h>

static char log_level_char[] = {
	'I',
	'W',
	'E',
};

static const char* file_name_from_path(const char* path) {
	const char* file_name = path;
	while (*(path++)) {
		if (*path == '/' || *path == '\\') {
			file_name = path + 1;
		}
	}
	return file_name;
}

static void print_string(const char* str) {
	while (*str) {
		hw_serial_write(*str);
		str++;
	}
}

void logging_printf(log_level_t level, const char* file, uint16_t line, const char* fmt, ...) {
	/* Print prefix */
	hw_serial_write(log_level_char[level]);
	hw_serial_write(' ');
	print_string(file_name_from_path(file));
	hw_serial_write(':');
	char line_str[5];
	snprintf(line_str, 5, "%d", line);
	print_string(line_str);
	hw_serial_write(' ');

	/* Print user string */
	char str[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(str, 128, fmt, args);
	va_end(args);
	print_string(str);
}
