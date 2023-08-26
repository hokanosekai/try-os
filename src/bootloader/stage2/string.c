#include "string.h"
#include <stdint.h>
#include <stddef.h>

const char* strchr(const char* str, char c)
{
  if (str == NULL)
    return NULL;

  while (*str != c) {
    if (!*str++)
      return NULL;
  }
  return str;
}

char* strcpy(char* dest, const char* src) {
  if (dest == NULL || src == NULL)
    return NULL;

  char* ret = dest;
  while (*src) {
    *dest++ = *src++;
  }
  *dest = '\0';
  return ret;
}

unsigned strlen(const char* str) {
  if (str == NULL)
    return 0;

  unsigned len = 0;
  while (*str++) {
    len++;
  }
  return len;
}