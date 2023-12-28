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

static uint32_t g_ms_since_midnight = 0;

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
	uint32_t now_ms = g_ms_since_midnight + timer0_now_ms();
	uint32_t hour = (now_ms / (1000L * 60L * 60L)) % 24L;
	uint32_t minutes = (now_ms / (1000L * 60L)) % 60L;
	uint32_t seconds = (now_ms / 1000L) % 60L;
	uint32_t milliseconds = now_ms % 1000L;

	return snprintf(str_buf, str_buf_len, "%02lu:%02lu:%02lu:%03lu", hour, minutes, seconds, milliseconds);
}

static void serial_printf(const char* fmt, ...) {
	char str[128];
	va_list args;
	va_start(args, fmt);
	vsnprintf(str, 128, fmt, args);
	va_end(args);

	hw_serial_print(str);
}

void logging_initialize(void) {
	// serial_printf("[ Logging ] Logging initialized, waiting for wall clock time.\n");

	// char input_buf[64] = { 0 };
	// const uint32_t timeout_ms = 100;
	// const uint32_t start_ms = timer0_now_ms();
	// while (true) {
	// 	/* Read input, look for clock time */
	// 	hw_serial_read_string(input_buf, 64);
	// 	if (string_starts_with(input_buf, "TIMENOW")) {
	// 		/* Received clock time */
	// 		int offset = strlen("TIMENOW ");
	// 		g_ms_since_midnight = strtol(input_buf + offset, NULL, 10);
	// 		serial_printf("[ Logging ] Received wall clock time.\n", g_ms_since_midnight);
	// 		break;
	// 	}

	// 	/* Timed out, abort */
	// 	const uint32_t now_ms = timer0_now_ms();
	// 	if (now_ms - start_ms > timeout_ms) {
	// 		serial_printf("[ Logging ] Timed out on getting wall clock time (waited %lu milliseconds).\n", timeout_ms);
	// 		break;
	// 	}
	// }
}

#include "gpio.h"
#define ONBOARD_LED \
	(gpio_pin_t) { .port = &PORTB, .num = 5 }

void logging_printf(log_level_t level, const char* file, int line, const char* fmt, ...) {
	char str[128];
	int offset = 0;

	gpio_pin_set(ONBOARD_LED);

	/* Print prefix */
	// offset += snprintf(str + offset, 128 - offset, "[");
	// offset += snprintf_time(str + offset, 128 - offset);
	// offset += snprintf(str + offset, 128 - offset, " ");
	// offset += snprintf(str + offset, 128 - offset, "%s%s%s %s:%d] ", log_level_color[level], log_level_str[level], COLOR_RESET, file_name_from_path(file), line);

	/* Print user string */
	va_list args;
	va_start(args, fmt);
	vsnprintf(str + offset, 128 - offset, fmt, args);
	va_end(args);

	gpio_pin_clear(ONBOARD_LED);

	hw_serial_print(str);
}
