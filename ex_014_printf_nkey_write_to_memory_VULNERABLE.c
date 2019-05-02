#include <stdio.h>

int main () {
  int x = 0;
  printf ("12345%n12345\n", &x);
  printf ("%d\n", x);
  return 0;
}
