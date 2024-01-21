#include "logging.h"

#include "hardware/hardware_serial.h"
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
		HardwareSerial_write(*str);
		str++;
	}
}

void logging_printf(log_level_t level, const char* file, uint16_t line, const char* fmt, ...) {
	/* Print prefix */
	HardwareSerial_write(log_level_char[level]);
	HardwareSerial_write(' ');
	print_string(file_name_from_path(file));
	HardwareSerial_write(':');
	char line_str[5];
	snprintf(line_str, 5, "%d", line);
	print_string(line_str);
	HardwareSerial_write(' ');

	/* Print user string */
	char str[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(str, 128, fmt, args);
	va_end(args);
	print_string(str);
}
