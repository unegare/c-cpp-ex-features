#include <iostream>
#include <unistd.h>

int main () {
  std::cout << "\033[31;42mTEXT\033[0m";
  std::cout.flush();
  std::cout << std::endl;

  int p;
  for(p=0;p<100;p++)
  {
    std::cout << "\033[A\033[2K[";
    for(int i=0;i<p;i++)
      std::cout << "=";
    std::cout << ">";
    for(int i=p;i<100;i++)
      std::cout << " ";
    std::cout << "]" << std::endl;
    usleep(100000);
  }
  return 0;
}
