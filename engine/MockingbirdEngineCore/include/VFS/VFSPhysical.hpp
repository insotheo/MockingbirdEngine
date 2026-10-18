#pragma once

#include "VFS/VFSDriver.hpp"
#include "VFS/VFSStream.hpp"
#include <filesystem>
#include <fstream>
#include <memory>

namespace Mockingbird::Core {
class VFSPhysicalStream : public VFSStream {
public:
  explicit VFSPhysicalStream(const std::filesystem::path &systemPath);

  ~VFSPhysicalStream() override {
    if (m_File.is_open())
      m_File.close();
  }

  inline bool IsValid() const { return m_File.is_open(); }

  size_t Read(void *buffer, size_t size) override;
  bool Seek(int64_t offset, StreamPos pos) override;

  inline int64_t Tell() const override {
    if (!m_File.is_open())
      return 0;
    return static_cast<int64_t>(m_File.tellg());
  }

  inline size_t GetSize() const override { return m_Size; }

private:
  mutable std::fstream m_File;
  size_t m_Size;
};

class VFSPhysicalDriver : public VFSDriver {
public:
  explicit VFSPhysicalDriver(const std::string &root) {
    m_RootPath =
        std::filesystem::absolute(root).lexically_normal().make_preferred();

    if (!m_RootPath.string().empty() &&
        m_RootPath.string().back() !=
            std::filesystem::path::preferred_separator) {
      m_RootPath =
          m_RootPath.string() + std::filesystem::path::preferred_separator;
    }
  }

  std::unique_ptr<VFSStream> OpenFile(const std::string &path) override;
  bool FileExists(const std::string &path) override;
  void ListDirectory(const std::string &path,
                     std::vector<std::string> &outFiles) override;

private:
  std::string ResolveAndSanitize(const std::string &relPath) const;

  std::filesystem::path m_RootPath;
};
} // namespace Mockingbird::Core
