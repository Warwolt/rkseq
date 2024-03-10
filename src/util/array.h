#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

#define array_length(x) (sizeof(x) / sizeof((x)[0]))
#define clear_array(x)                             \
	for (size_t i = 0; i < array_length(x); i++) { \
		x[i] = 0;                                  \
	}

#endif /* ARRAY_H */
