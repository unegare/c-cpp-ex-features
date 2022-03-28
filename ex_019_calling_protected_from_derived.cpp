#include <iostream>

template<typename T>
class Base {
  T val;
protected:
  void _inc();
  void _add(const T&);
public:
  Base();
  ~Base();
  const T& get() const;
};

template<typename T>
Base<T>::Base(): val() {}

template<typename T>
Base<T>::~Base() {}

template<typename T>
const T& Base<T>::get() const {
  return val;
}

template<typename T>
void Base<T>::_inc() {
  val++;
}

template<typename T>
void Base<T>::_add(const T& extra) {
  val += extra;
}

template<typename T>
class Derived: public Base<T> {
public:
  using Base<T>::Base;

  void inc();
  void add(const T&);
};



template<typename T>
void Derived<T>::inc() {
//  _inc(); // ERROR!!! since for no arguments types cannot be derived
//          // error: there are no arguments to ‘_inc’ that depend on a template parameter, so a declaration of ‘_inc’ must be available 

  this->_inc(); // Ok
  Base<T>::_inc(); // Ok
}

template<typename T>
void Derived<T>::add(const T& extra) {
//  _add(extra); //  error: ‘_add’ was not declared in this scope, and no declarations were found by argument-dependent lookup at the point of instantiation
//               //  note: declarations in dependent base ‘Base<long unsigned int>’ are not found by unqualified lookup

  this->_add(extra); // Ok
  Base<T>::_add(extra); // Ok
} 

int main() {
  Derived<size_t> d;
  std::cout << d.get() << std::endl;
  d.inc();
  std::cout << d.get() << std::endl;
  d.add(5);
  std::cout << d.get() << std::endl;
  return 0;
}
