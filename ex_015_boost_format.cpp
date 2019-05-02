#include <iostream>
#include <cstring>
#include <boost/format.hpp>

int main () {
  int x = 10;
  std::cout << boost::format("x = %d | &x = %p") % x % &x << std::endl;
  return 0;
}
