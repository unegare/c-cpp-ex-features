#include <iostream>

struct data {
  char arr[2];
  int x;
};

int func (int val) {
  static data d;
  int bval = d.x;
  d.x = val;
  return bval;
}

int main () {
  std::cout << func(1) << std::endl;
  std::cout << func(10) << std::endl;
  std::cout << func(6) << std::endl;
  return 0;
}
