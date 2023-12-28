#include "logging.h"

#include "bits.h"
#include "hw_serial.h"
#include "timer0.h"

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

void logging_printf(log_level_t level, const char* file, uint16_t line, const char* fmt, ...) {
	/* Print prefix */
	hw_serial_putc(log_level_char[level]);
	hw_serial_putc(' ');
	hw_serial_print(file_name_from_path(file));
	hw_serial_putc(':');
	char line_str[5];
	snprintf(line_str, 5, "%d", line);
	hw_serial_print(line_str);
	hw_serial_putc(' ');

	/* Print user string */
	char str[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(str, 128, fmt, args);
	va_end(args);
	hw_serial_print(str);
}
