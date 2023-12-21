/*
  logging.c - Serial logging for the Arduino Uno in C
  Copyright (c) 2006 Rasmus Källqvist.  All right reserved.

  logging.c is heavily based on HardwareSerial.cpp and
  HardwareSerial0.cpp by Nicholas Zambetti.

  ------------------------------------------------------------

  HardwareSerial.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
  Modified 18 December 2023 by Rasmus Källqvist
*/

#include "logging.h"

#include "hw_serial.h"
#include "timer.h"

#include <avr/io.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/atomic.h>

#define clear_bit(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define set_bit(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

/* ----------------------------- String utility ----------------------------- */
static bool string_starts_with(const char* str, const char* prefix) {
	return strncmp(prefix, str, strlen(prefix)) == 0;
}

static const char* file_name_from_path(const char* path) {
	const char* file_name = path;
	while (*(path++)) {
		if (*path == '/' || *path == '\\') {
			file_name = path + 1;
		}
	}
	return file_name;
}

/* ------------------------------- Public API ------------------------------- */
#define COLOR_GREEN "\033[32m"
#define COLOR_RED "\033[31m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RESET "\033[0m"

static unsigned long g_ms_since_midnight = 0;

static const char* log_level_str[] = {
	"INFO",
	"WARNING",
	"ERROR",
};

static const char* log_level_color[] = {
	COLOR_GREEN,
	COLOR_YELLOW,
	COLOR_RED,
};

static int snprintf_time(char* str_buf, size_t str_buf_len) {
	unsigned long now_ms = g_ms_since_midnight + timer_now_ms();
	unsigned long hour = (now_ms / (1000L * 60L * 60L)) % 24L;
	unsigned long minutes = (now_ms / (1000L * 60L)) % 60L;
	unsigned long seconds = (now_ms / 1000L) % 60L;
	unsigned long milliseconds = now_ms % 1000L;

	return snprintf(str_buf, str_buf_len, "%02lu:%02lu:%02lu:%03lu", hour, minutes, seconds, milliseconds);
}

static void serial_printf(const char* fmt, ...) {
	char str[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(str, 128, fmt, args);
	va_end(args);

	serial_print(str);
}

void logging_initialize() {
	serial_printf("[ Logging ] Logging initialized, waiting for wall clock time.\n");

	char input_buf[64] = { 0 };
	const unsigned long timeout_ms = 100;
	const unsigned long start_ms = timer_now_ms();
	while (true) {
		/* Read input, look for clock time */
		serial_read_string(input_buf, 64);
		if (string_starts_with(input_buf, "TIMENOW")) {
			/* Received clock time */
			int offset = strlen("TIMENOW ");
			g_ms_since_midnight = strtol(input_buf + offset, NULL, 10);
			serial_printf("[ Logging ] Received wall clock time.\n", g_ms_since_midnight);
			break;
		}

		/* Timed out, abort */
		const unsigned long now_ms = timer_now_ms();
		if (now_ms - start_ms > timeout_ms) {
			serial_printf("[ Logging ] Timed out on getting wall clock time (waited %lu milliseconds).\n", timeout_ms);
			break;
		}
	}
}

void logging_printf(log_level_t level, const char* file, int line, const char* fmt, ...) {
	char str[128];
	int offset = 0;

	/* Print prefix */
	offset += snprintf(str + offset, 128 - offset, "[");
	offset += snprintf_time(str + offset, 128 - offset);
	offset += snprintf(str + offset, 128 - offset, " ");
	offset += snprintf(str + offset, 128 - offset, "%s%s%s %s:%d] ", log_level_color[level], log_level_str[level], COLOR_RESET, file_name_from_path(file), line);

	/* Print user string */
	va_list args;
	va_start(args, fmt);
	vsnprintf(str + offset, 128 - offset, fmt, args);
	va_end(args);

	serial_print(str);
}
