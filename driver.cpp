#include <random>
#include <chrono>
#include <array>
#include <iostream>
#include <algorithm>

int main()
{
  const auto repeat_times = 2000U;
  const auto alloc_count = 10000U;
  const auto max_alloc_size = 1000U;
  auto clock = std::chrono::high_resolution_clock();

  std::cout << "Profiling random size allocation/deletion between 0 to " << max_alloc_size << " bytes block " 
    << alloc_count * repeat_times << " times..." << std::endl;

  // generate random size for testing
  std::array<int, alloc_count> sizes;
  std::generate(sizes.begin(), sizes.end(), [max_alloc_size]() {return rand() % max_alloc_size;});
  
  // measure original allocation speed
  auto start = clock.now();
  for (int j = 0; j < repeat_times; ++j)
  {
    for (int i = 0; i < alloc_count; ++i)
    {
      auto addr = malloc(sizes[i]);
      free(addr);
    }
  }
  auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - start);
  std::cout << "Original allocation performance:" << std::endl;
  std::cout << diff.count() << " milliseconds" << std::endl;

  // measure improved allocation speed
  auto start2 = clock.now();
  for (int j = 0; j < repeat_times; ++j)
  {
    for (unsigned i = 0; i < alloc_count; ++i)
    {
      auto addr = new char[sizes[i]];
      delete[] addr;
    }
  }
  auto diff2 = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now() - start2);
  std::cout << "Improved allocation performance:" << std::endl;
  std::cout << diff2.count() << " milliseconds" << std::endl;

  std::cout << "Improved allocation is " << (double)diff.count() / diff2.count() << " times faster." << std::endl;
}