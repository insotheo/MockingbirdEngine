#pragma once

#include <cstdint>

namespace Mockingbird::Core {
struct Time {
  float DeltaTime = 0.0f;
  double TotalTime = 0.0f;
  uint64_t FrameCount = 0;
};
} // namespace Mockingbird::Core
