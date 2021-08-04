#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <vector>
#include <iterator>

#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/sort.h>
#include <thrust/copy.h>

int main() {
  std::ios::sync_with_stdio(false);

  srand(time(NULL));

  //32M random values
  thrust::host_vector<int> h_vec(32 << 20);
  thrust::generate(begin(h_vec), end(h_vec), rand);
  
  auto t1 = std::chrono::high_resolution_clock::now();

  thrust::device_vector<int> d_vec = h_vec;
  thrust::sort(d_vec.begin(), d_vec.end());
  thrust::copy(d_vec.begin(), d_vec.end(), h_vec.begin());

  auto t2 = std::chrono::high_resolution_clock::now();

  //Duration (cuda): 676ms (GTX 1080 TI, driver 470, cuda 11.4, first time)
  //Duration (cuda): 110ms (GTX 1080 TI, driver 470, cuda 11.4, second time and futher)
  std::cout << "Duration (cuda): " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << "ms" << std::endl;



  std::vector<int> v(h_vec.begin(), h_vec.end());

  auto t3 = std::chrono::high_resolution_clock::now();

  std::sort(begin(v), end(v));

  auto t4 = std::chrono::high_resolution_clock::now();

  //Duration (host): 6919ms (i3 8350K @ 4.00 GHz, cache 8MB, DDR4 8Gb x2 3000 MHz)
  std::cout << "Duration (host): " << std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count() << "ms" << std::endl;

  std::copy(h_vec.begin(), std::next(h_vec.begin(), 10), std::ostream_iterator<int>(std::cout, ", "));
  std::cout << std::endl;

  std::copy(begin(v), std::next(begin(v), 10), std::ostream_iterator<int>(std::cout, ", "));
  std::cout << std::endl;

  return 0;
}
