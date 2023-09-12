#ifndef __STRING_H__
#define __STRING_H__

#include <libc/stddef.h>

const char* strchr(const char* str, char c);
char* strcpy(char* dest, const char* src);
size_t strlen(const char* str);

#endif