#include <iostream>
#include <vector>

//---------------------------------------------------
//------------------ like in std --------------------
//---------------------------------------------------

template<bool condition, typename T>
struct EnableIf;

template<typename T>
struct EnableIf<true, T> {
  using type = T;
};

template<typename T, typename U>
struct IsSame {
  static constexpr bool value = false;
};

template<typename T>
struct IsSame<T,T> {
  static constexpr bool value = true;
};

//---------------------------------------------------
//----------------- does not exist in std -----------
//---------------------------------------------------

template<typename T>
struct has_cbegin_cend {
  using yes = char;
  using no = struct { yes arr[2]; };
  
  template<typename U>
  static auto test(U* u) -> decltype((*u).cbegin(), (*u).cend(), yes());
  static no test(...);

  enum {value = (sizeof(yes) == sizeof test((T*) 0))};
};

//---------------------------------------------------
//----------------- decltype & declval --------------
//---------------------------------------------------

template<typename T>
decltype(begin(std::declval<T>()), end(std::declval<T>()), int()) // only if 'begin' and 'end' can be called
function (T const& a, T const& b) {
  if (a.size() == 0 || b.size() == 0) {
    return 0;
  }
  return a[0] + b[0];
}

//---------------------------------------------------
//---------------------------------------------------
//---------------------------------------------------


template<typename T>
typename EnableIf<IsSame<T, int>::value, T>::type
//typename std::enable_if<std::is_same<T, int>::value, T>::type
func (T a) {
  std::cout << __FUNCTION__ << " : int" << std::endl;
  return a;
}

template<typename T>
//typename EnableIf<!IsSame<T, int>::value, T>::type
typename std::enable_if<!std::is_same<T, int>::value, T>::type
func (T a) {
  std::cout << __FUNCTION__ << " : not int" << std::endl;
  return a;
}


template<typename T>
typename std::enable_if<!has_cbegin_cend<T>::value, void>::type
function (T const& a, T const& b) {
  std::cout << __FUNCTION__ << " : it is not iterable object" << std::endl;
}

int main () {
  std::vector<int> v1({1,2}), v2({3,4});
  std::cout << function(v1, v2) << std::endl;
  function(1,2);
  func(1);
  func(2.0);
  return 0;
}
