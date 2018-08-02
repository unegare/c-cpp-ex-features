#include <iostream>
#include <exception>
#include <string>
#include <cxxabi.h>

#define Wfpermissive

int main () {
  try {
//    void *ex_simple = __cxxabiv1::__cxa_allocate_exception(0);
//    __cxxabiv1::__cxa_throw(ex_simple, 0, 0);
    void *ex = __cxxabiv1::__cxa_allocate_exception(sizeof(std::runtime_error));
    new (ex) std::runtime_error("WoW!");
    __cxxabiv1::__cxa_throw(ex, (std::type_info*)(&typeid(*((std::runtime_error*)ex))), 0);
  } catch (std::runtime_error &err) {
    std::cout << err.what() << std::endl;
  } catch (...) {
    std::cout << "Exception" << std::endl;
  }
  return 0;
}
