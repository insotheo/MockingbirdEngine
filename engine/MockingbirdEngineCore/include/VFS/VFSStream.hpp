#pragma once

#include "VFS/VFSInfos.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace Mockingbird::Core {
enum class StreamPos { Current, End, Begin };

class VFSStream {
public:
  virtual ~VFSStream() = default;

  virtual size_t Read(void *buffer, size_t size) = 0;
  virtual bool Seek(int64_t offset, StreamPos pos) = 0;
  virtual int64_t Tell() const = 0;
  virtual size_t GetSize() const = 0;

  std::vector<uint8_t> GetBytes();
  std::string GetString();
  ImageFile GetImage();
};
} // namespace Mockingbird::Core
