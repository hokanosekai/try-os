#pragma once

#include "stdbool.h"

/**
 * @brief Clears the screen.
 */
void cls(void);

/**
 * @brief Writes a character to the teletype.
 */
void putc(char c);

/**
 * @brief Writes a string to the teletype.
 */
void puts(const char* s);

/**
 * @brief Writes a formatted string to the teletype.
 */
void _cdecl printf(const char* fmt, ...);

/**
 * @brief Writes a formatted string to the teletype.
 */
int* printf_number(int* argp, int length, int radix, bool sign);

/**
 * @brief Reads a character from the teletype.
 */
char getc(void);

/**
 * @brief Reads a string from the teletype.
 */
char* gets(char* s, int n);

/**
 * @brief Reads a string from the teletype with a defined size.
 */
char* fgets(char* s, int size);

/**
 * @brief Reads a formatted string from the teletype.
 */
int _cdecl scanf(const char* fmt, ...);
