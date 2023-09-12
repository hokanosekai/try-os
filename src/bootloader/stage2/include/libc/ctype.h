#ifndef __CTYPE_H__
#define __CTYPE_H__

#include <libc/stdbool.h>

bool isdigit(char c);
bool isalpha(char c);
bool isalnum(char c);
bool iscntrl(char c);
bool islower(char c);
bool isupper(char c);

char tolower(char c);
char toupper(char c);

#endif
