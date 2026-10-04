#pragma once

#include <cstdint>
#include <string>

namespace Mockingbird::SDL::Windowing {
struct WindowInfo {
  uint32_t Width;
  uint32_t Height;
  std::string Title;
};
} // namespace Mockingbird::SDL::Windowing
