#pragma once

#include <cstdint>
#include <string>

namespace Pumpkin::SDL::Windowing {
struct WindowInfo {
  uint32_t Width;
  uint32_t Height;
  std::string Title;
};
} // namespace Pumpkin::SDL::Windowing
