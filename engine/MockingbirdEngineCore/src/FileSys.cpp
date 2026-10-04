#include "Core/FileSys.hpp"

#include "Core/Log.hpp"
#include <fstream>

namespace Mockingbird::Core {
std::vector<std::byte> LoadFileBytes(const std::string &filepath) {
  std::ifstream file(filepath, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    ME_LOG_CORE_ERROR("Failed to open file: {}", filepath);
    return {};
  }

  std::streamsize fileSize = file.tellg();

  std::vector<std::byte> buffer(fileSize);

  file.seekg(0, std::ios::beg);
  if (!file.read(reinterpret_cast<char *>(buffer.data()), fileSize)) {
    ME_LOG_CORE_ERROR("Failed to read file completly: {}", filepath);
    return {};
  }

  return buffer;
}
} // namespace Mockingbird::Core
