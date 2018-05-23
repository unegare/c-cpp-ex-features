#include <stdio.h>
#include <stdlib.h>

void func1 () {
  printf("%s\n", __FUNCTION__);
}

void func2 () {
  printf("%s\n", __FUNCTION__);
}

int main () {
  if (atexit(func1) != 0) {
    printf("registration failure 1\n");
    return 0;
  }
  if (atexit(func2) != 0) {
    printf("registration failure 2\n");
    return 0;
  }
  printf ("body\n");
  return 0;
}
