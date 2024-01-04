#ifndef UTIL_MATH_H
#define UTIL_MATH_H

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
#define clamp(x, minval, maxval) min(max((x), (minval)), (maxval))
#define clamped_subtract(x, y) ((x) > (y) ? x - y : 1)

#endif /* UTIL_MATH_H */
