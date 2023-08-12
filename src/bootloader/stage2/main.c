#include "stdio.h"

void _cdecl cstart_(uint16_t bootDrive) {
  char name[32];

  cls();

  puts("What is your name? ");
  fgets(name, sizeof(char) * 32);

  cls();

  printf("Hello %s!\n\r", name);
  printf("Boot drive: %d\n\r", bootDrive);

  while (true) {
    char c = getc();
    if (c == 'A') {
      printf("test\n\r");
    } else {
      printf("You pressed %c!\n\r", c);
    }
  }
}