#pragma once

#include <stdint.h>

void cls();
void putc(char c);
void puts(const char* str);
void printf(const char* format, ...);
void print_buffer(const char* msg, const void* buffer, uint32_t size);

void getc(char* c);