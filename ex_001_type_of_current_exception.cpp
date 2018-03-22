#include <iostream>
#include <string>
#include <exception>
#include <stdexcept>
#include <typeinfo>
#include <cxxabi.h>

namespace Foo {
class Foo {

};
}

int main () {
  Foo::Foo f;
  try {
    throw f;
  } catch (...) {
    std::cout << __cxxabiv1::__cxa_current_exception_type()->name() << std::endl; // for | c++filt -t

    int status = 0;
    char *buff = __cxxabiv1::__cxa_demangle(__cxxabiv1::__cxa_current_exception_type()->name(), NULL, NULL, &status);
    std::string errtype = buff;
    std::free(buff);   
    std::cout << errtype << std::endl;
  }
  return 0;
}
