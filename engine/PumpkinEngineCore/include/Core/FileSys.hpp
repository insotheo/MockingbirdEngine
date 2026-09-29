#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace Pumpkin::Core {
std::vector<std::byte> LoadFileBytes(const std::string &filepath);
}
