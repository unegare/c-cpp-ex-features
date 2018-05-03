#include <iostream>
#include <vector>

//WITHOUT namespace claiming !

int main () {
  operator<<(std::cout, "Test");
  endl(std::cout);
  
  std::vector<int> v({0,1,2,3});
  for (auto it = begin(v); it != end(v); it++) {
    operator<<(std::cout, std::to_string(*it));
  }
  endl(std::cout);
  return 0;
}
