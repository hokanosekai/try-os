#ifndef __BINARY_H__
#define __BINARY_H__

#define FLAG_SET(flags, flag)   ((flags) |= (flag))
#define FLAG_UNSET(flags, flag) ((flags) &= ~(flag))
#define FLAG_ISSET(flags, flag) ((flags) & (flag))

#endif