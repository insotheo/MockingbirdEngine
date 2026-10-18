#include "VFS/VFSPhysical.hpp"

#include "Core/Log.hpp"
#include "VFS/VFSStream.hpp"

namespace Mockingbird::Core {

VFSPhysicalStream::VFSPhysicalStream(const std::filesystem::path &systemPath)
    : m_File(systemPath, std::ios::in | std::ios::binary), m_Size(0) {
  if (m_File.is_open()) {
    m_File.seekg(0, std::ios::end);
    m_Size = static_cast<size_t>(m_File.tellg());
    m_File.seekg(0, std::ios::beg);
  } else {
    ME_LOG_CORE_ERROR("VFS: Failed to open file: {}", systemPath.string());
  }
}

size_t VFSPhysicalStream::Read(void *buffer, size_t size) {
  if (!m_File.is_open() || size == 0)
    return 0;

  m_File.read(reinterpret_cast<char *>(buffer), size);
  return static_cast<size_t>(m_File.gcount());
}

bool VFSPhysicalStream::Seek(int64_t offset, StreamPos pos) {
  if (!m_File.is_open())
    return false;

  std::ios_base::seekdir dir;
  if (pos == StreamPos::Current)
    dir = std::ios::cur;
  else if (pos == StreamPos::Begin)
    dir = std::ios::beg;
  else if (pos == StreamPos::End)
    dir = std::ios::end;

  m_File.clear();
  m_File.seekg(offset, dir);
  return !m_File.fail();
}

std::string
VFSPhysicalDriver::ResolveAndSanitize(const std::string &relPath) const {
  std::filesystem::path securePath = m_RootPath / relPath;
  securePath = securePath.lexically_normal().make_preferred();

  if (securePath.string().find(m_RootPath.string()) != 0) {
    ME_LOG_CORE_WARN("VFS Security Warning! Traversal attempt blocked: {}",
                     relPath);
    return "";
  }
  return securePath;
}

std::unique_ptr<VFSStream>
VFSPhysicalDriver::OpenFile(const std::string &path) {
  std::filesystem::path systemPath = ResolveAndSanitize(path);
  if (systemPath.empty())
    return nullptr;

  auto stream = std::make_unique<VFSPhysicalStream>(systemPath);
  if (!stream->IsValid())
    return nullptr;

  return std::move(stream);
}

bool VFSPhysicalDriver::FileExists(const std::string &path) {
  std::filesystem::path systemPath = ResolveAndSanitize(path);
  if (systemPath.empty())
    return false;
  return std::filesystem::is_regular_file(systemPath);
}

void VFSPhysicalDriver::ListDirectory(const std::string &path,
                                      std::vector<std::string> &outFiles) {
  std::filesystem::path systemPath = ResolveAndSanitize(path);
  if (systemPath.empty() || !std::filesystem::is_directory(systemPath))
    return;

  for (const auto &entry : std::filesystem::directory_iterator(systemPath)) {
    std::string rel = std::filesystem::relative(entry.path(), m_RootPath);
    outFiles.push_back(rel);
  }
}

} // namespace Mockingbird::Core
