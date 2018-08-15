#include <iostream>
#include <regex>

int main () {
  const char *row = R"(function (uint, uint, uint[], struct Temp.data storage pointer) public returns (uint) )";
  try {
    std::regex_token_iterator<const char *>::regex_type re(R"(\([a-zA-Z0-9\[\],\. ]+\))");
    std::regex_token_iterator<const char *> next(row, row + strlen(row), re);
    std::regex_token_iterator<const char*> end;
    
    for (; next != end; ++next) {
      std::cout << "match == \"" << next->str() << "\"" << std::endl;
    }
  } catch (std::exception &err) {
    std::cout << err.what() << std::endl;
  }
  return 0;
}
