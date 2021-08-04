#include <iostream>
#include <string>

class MyClass1 {
public:
  MyClass1() {std::cout << __PRETTY_FUNCTION__ << std::endl;}
  ~MyClass1() {std::cout << __PRETTY_FUNCTION__ << std::endl;}

  int field{10};
};

std::ostream& operator<< (std::ostream& os, const MyClass1& ref) {
  return os << "MyClass1 { field: " << ref.field << " }";
}

class MyClass2 {
public:
  MyClass2() = delete;
  MyClass2(const MyClass1& mc): ref{mc} {std::cout << __PRETTY_FUNCTION__ << std::endl;}
  ~MyClass2() {std::cout << __PRETTY_FUNCTION__ << std::endl;}
  const MyClass1& ref;
};

class Sandbox {
public:
  Sandbox(const std::string& s) : member(s) {}
//  Sandbox(std::string&&) = delete;
  const std::string member;
};

std::string f() {return "asdf";}

int f1() {static int x = 0; std::cout << __PRETTY_FUNCTION__ << std::endl; return x++;}

void f2(int x = f1()) {std::cout << __PRETTY_FUNCTION__ << ": x = " << x << std::endl;}

int main() {
  std::cout << "The answer is: " << Sandbox(std::string("four")).member << std::endl; //UB string will be destructed as constructor is finished
  Sandbox sb(std::string("five")); // string destructed as constructor is finished
  std::cout << "The other answer is: " << sb.member << std::endl; //UB

  const std::string& res = f(); // OK, since it is a const lvalue reference, lifetime prolonged until the reference fall out of scope

  std::cout << res << std::endl; //OK

  MyClass2 mc2{MyClass1()}; //MyClass1 is destructed as MyClass2 constructor is finished

  std::cout << "mc2.ref: " << mc2.ref << std::endl; //UB

  f2(); //
  f2(); //f1 is called every time

  return 0;
}

