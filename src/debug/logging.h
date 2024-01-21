#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>

typedef enum {
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
} log_level_t;

#ifdef NDEBUG
#define LOG_INFO(...)
#define LOG_WARNING(...)
#define LOG_ERROR(...)
#else
#define LOG_INFO(...) Logging_printf(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) Logging_printf(LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) Logging_printf(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#endif

void Logging_printf(log_level_t level, const char* file, uint16_t line, const char* fmt, ...);

#endif /* LOGGING_H */
