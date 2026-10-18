#pragma once

#include "VFS/VFSStream.hpp"
#include <memory>
#include <vector>

namespace Mockingbird::Core {
class VFSDriver {
public:
  virtual ~VFSDriver() = default;

  virtual std::unique_ptr<VFSStream> OpenFile(const std::string &path) = 0;
  virtual bool FileExists(const std::string &path) = 0;
  virtual void ListDirectory(const std::string &path,
                             std::vector<std::string> &outFiles) = 0;
};
} // namespace Mockingbird::Core
