#include <iostream>
#include <vector>

//WITHOUT namespace claiming !

namespace my {
  class data {
    public:
    uint32_t a;
  };
  void set (class data& d, uint32_t val);
};

int main () {
  operator<<(std::cout, "Test");
  endl(std::cout);
  
  std::vector<int> v({0,1,2,3});
  for (auto it = begin(v); it != end(v); it++) {
    operator<<(std::cout, std::to_string(*it));
  }
  endl(std::cout);

  class my::data d;

  set(d, 10);

  return 0;
}

void my::set (class my::data& d, uint32_t val) {
  d.a = val;
  std::cout << "I'm" << std::endl;
}
