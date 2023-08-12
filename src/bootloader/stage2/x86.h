#include "stdint.h"

/**
 * @brief Divides a 64-bit unsigned integer by a 32-bit unsigned integer.
 */
void _cdecl x86_div64_32(uint64_t dividend, uint32_t divisor, uint64_t* quotient, uint32_t* remainder);

/**
 * @brief Clear the screen.
 */
void _cdecl x86_Video_ClearScreen(void);

/**
 * @brief Writes a character to the teletype.
 */
void _cdecl x86_Video_WriteCharTeletype(char c, uint8_t page);

/**
 * @brief Reads a character from the teletype.
 */
char _cdecl x86_Video_ReadCharTeletype(uint8_t page);