#ifndef __MATH_H__
#define __MATH_H__

#include <libc/stdint.h>

#define abs(x)      ((x) < 0 ? -(x) : (x))
#define min(a,b)    ((a) < (b) ? (a) : (b))
#define max(a,b)    ((a) > (b) ? (a) : (b))

#define PI          3.14159265358979323846

#define deg2rad(x)  ((x) * PI / 180.0)
#define rad2deg(x)  ((x) * 180.0 / PI)

#endif

