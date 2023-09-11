#ifndef __STDARG_H__
#define __STDARG_H__

typedef char* va_list;

#define VA_SIZE(type) ((sizeof(type) + sizeof(int) - 1) & ~(sizeof(int) - 1))

#define va_start(ap, last) (ap = (va_list)&last + sizeof(last))

#define va_arg(ap, type) (ap += VA_SIZE(type), *(type*)(ap - VA_SIZE(type)))

#define va_end(ap)

#endif