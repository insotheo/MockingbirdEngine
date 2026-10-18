#pragma once

#include "VFS/VFSDriver.hpp"
#include "VFS/VFSStream.hpp"
#include <filesystem>
#include <memory>
#include <string>

namespace Mockingbird::Core {
class VFS {
public:
  inline void Mount(const std::string &virtualPath,
                    std::shared_ptr<VFSDriver> driver) {
    m_Mounts.insert(m_Mounts.begin(), {virtualPath, driver});
  };

  std::unique_ptr<VFSStream> Open(const std::string &path);
  bool FileExists(const std::string &path);

private:
  struct MountPoint {
    std::string VirtualPath;
    std::shared_ptr<VFSDriver> Driver;
  };

private:
  std::vector<MountPoint> m_Mounts;
};

std::filesystem::path GetExecutableDirectory();

} // namespace Mockingbird::Core
