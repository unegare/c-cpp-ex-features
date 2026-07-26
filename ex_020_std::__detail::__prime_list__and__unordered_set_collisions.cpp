#include <algorithm>
#include <iostream>
#include <format>
#include <iterator>
#include <unordered_set>
#include <vector>
#include <cstdint>

namespace std {
  namespace __detail {
    extern const unsigned long __prime_list[];
  }
}

int main() {
  std::unordered_set<uint64_t> s;
  std::vector<uint64_t> bucket_sizes;
  bucket_sizes.push_back(s.bucket_count());
  for (uint64_t i = 0; i < 1000;  ++i) {
    s.insert(i);
//    std::cout << s.bucket_count() << std::endl;
    bucket_sizes.push_back(s.bucket_count());
  }
  sort(begin(bucket_sizes), end(bucket_sizes));
  bucket_sizes.erase(unique(begin(bucket_sizes), end(bucket_sizes)), end(bucket_sizes));
  copy(cbegin(bucket_sizes), cend(bucket_sizes),  std::ostream_iterator<uint64_t>(std::cout,  ", "));
  std::cout << std::endl;
  std::unordered_set<uint64_t> s2;
  for (uint64_t i = 0; i < 59; ++i) {
    const auto tmp = 13llu * 29 * 59 * i;
    s2.insert(tmp);
    std::cout << std::format("i: {} | s2.bucket_count(): {} | s2.bucket(13*29*59*i): {}", i,  s2.bucket_count(), s2.bucket(tmp)) << std::endl;
  }
  std::cout << std::format("number of elements placed in 0th bucket out of {} buckets: {}", s2.bucket_count(), s2.bucket_size(0)) << std::endl;

  std::cout << std::format(
      "-------------------------------------------------------------------------\n"
      "list of hardcoded primes in std::__detail::__prime_list:\n"
  ) << std::endl;

  copy(&std::__detail::__prime_list[0], &std::__detail::__prime_list[0] + 256,  std::ostream_iterator<unsigned long>(std::cout, ", "));
  std::cout << std::endl;
  return 0;
}

