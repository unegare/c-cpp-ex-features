#include <iostream>

void func (int *x) __attribute__ ((nonnull(1)));

void func (int *x) {
  std::cout << x << std::endl;
}

int main () {
  func(0);
  return 0;
}
