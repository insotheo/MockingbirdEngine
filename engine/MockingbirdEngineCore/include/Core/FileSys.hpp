#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Mockingbird::Core {
std::vector<uint8_t> LoadFileBytes(const std::string &filepath);
}
