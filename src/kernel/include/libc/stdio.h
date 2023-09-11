#ifndef __STDIO_H__
#define __STDIO_H__

#include <libc/stdint.h>

void cls();
void putc(char c);
void puts(const char* str);
void printf(const char* format, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t size);

void printOK();
void printERR();

void getc(char* c);

#endif