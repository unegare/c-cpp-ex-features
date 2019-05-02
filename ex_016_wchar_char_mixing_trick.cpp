#include <cstdio>
#include <iostream>

int main () {
  std::wcout << L"Hello" << std::flush;
  freopen(nullptr, "a", stdout);
  std::cout << " world\n" << std::flush;
}
