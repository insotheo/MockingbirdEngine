#pragma once

#include <cstdint>
#include <vector>

namespace Mockingbird::Core {
struct ImageFile {
  int Width = 0;
  int Height = 0;
  int Channels = 0;
  std::vector<uint8_t> Data;
};
} // namespace Mockingbird::Core
