#include "ctype.h"

bool isdigit(char c) {
  return c >= '0' && c <= '9';
}

bool isalpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool isupper(char c) {
  return c >= 'A' && c <= 'Z';
}

bool islower(char c) {
  return c >= 'a' && c <= 'z';
}

bool isalnum(char c) {
  return isdigit(c) || isalpha(c);
}

bool iscntrl(char c) {
  return c == '\a' || c == '\b' || c == '\f' || c == '\n' || c == '\r' ||
         c == '\t' || c == '\v';
}

char tolower(char c) {
  if (isupper(c)) {
    return c + 32;
  }
  return c;
}

char toupper(char c) {
  if (islower(c)) {
    return c - 32;
  }
  return c;
}