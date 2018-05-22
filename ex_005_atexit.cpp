#include <stdio.h>
#include <stdlib.h>

void func1 () {
  printf("%s\n", __FUNCTION__);
}

void func2 () {
  printf("%s\n", __FUNCTION__);
}

int main () {
  atexit(func1);
  atexit(func2);
  return 0;
}
