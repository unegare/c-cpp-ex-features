#include <stdlib.h>
#include <stdio.h>

void foo() {
  volatile long array[5] = {0,};
  long var1;
  long var2;
  long i = 0;
  
  for (i = 0; i < 20; ++i) {
    printf ("array[%ld] == %lx | address == %p\n", i, array[i], &array[i]);
  }

  printf("Enter two numbers:\n");
  scanf("%lx", &var1); // e.g. 7 (after canary)
  scanf("%lx", &var2); // e.g. addr of foo or main to cycle
  array[var1] = var2;
}

int main () {
  printf("%p\n", &main);
  printf("%p\n", &foo);
  foo();
__asm__ volatile ("":::"memory");
  return 0;
}
