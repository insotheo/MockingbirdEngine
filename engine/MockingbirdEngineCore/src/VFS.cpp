#include "VFS/VFS.hpp"
#include "VFS/VFSStream.hpp"

#include "Core/Log.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__)
#include <limits.h>
#include <unistd.h>
#endif

namespace Mockingbird::Core {

// Some functions connected to VFS
std::filesystem::path GetExecutableDirectory() {
#if defined(_WIN32)
  wchar_t buffer[MAX_PATH];
  DWORD length = GetModuleFileNameW(NULL, buffer, MAX_PATH);
  if (length == 0)
    return "";
  return std::filesystem::path(buffer).parent_path();
#elif defined(__linux__)
  char buffer[PATH_MAX];
  ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
  if (length == -1)
    return "";
  buffer[length] = '\0';
  return std::filesystem::path(buffer).parent_path();
#else
  return std::filesystem::current_path();
#endif
}

// VFSStream
std::vector<uint8_t> VFSStream::GetBytes() {
  size_t totalSize = GetSize();
  if (totalSize == 0)
    return {};

  Seek(0, StreamPos::Begin);

  std::vector<uint8_t> buffer(totalSize);
  size_t bytesRead = Read(buffer.data(), totalSize);
  if (bytesRead != totalSize) {
    buffer.resize(bytesRead);
  }
  return buffer;
}

std::string VFSStream::GetString() {
  std::vector<uint8_t> bytes = GetBytes();
  return std::string(bytes.begin(), bytes.end());
}

ImageFile VFSStream::GetImage() {
  ImageFile outImg{};

  std::vector<uint8_t> rawBuff = GetBytes();
  if (rawBuff.empty())
    return {};

  int width = 0, height = 0, channels = 0;
  unsigned char *pixels =
      stbi_load_from_memory(rawBuff.data(), static_cast<int>(rawBuff.size()),
                            &width, &height, &channels, STBI_rgb_alpha);
  if (!pixels) {
    ME_LOG_CORE_ERROR("Failed to load image through STB: {}",
                      stbi_failure_reason());
    return outImg;
  }

  outImg.Width = width;
  outImg.Height = height;
  outImg.Channels = channels;
  outImg.Data.assign(pixels, pixels + (width * height * 4));

  stbi_image_free(pixels);

  return outImg;
}

// VFS
std::unique_ptr<VFSStream> VFS::Open(const std::string &path) {
  for (const auto &mount : m_Mounts) {
    if (path.rfind(mount.VirtualPath, 0) == 0) {
      std::string relPath = path.substr(mount.VirtualPath.length());
      if (mount.Driver->FileExists(relPath))
        return mount.Driver->OpenFile(relPath);
    }
  }
  ME_LOG_CORE_ERROR("VFS: File not found: ", path);
  return nullptr;
}

bool VFS::FileExists(const std::string &path) {
  for (const auto &mount : m_Mounts) {
    if (path.rfind(mount.VirtualPath, 0) == 0) {
      std::string relPath = path.substr(mount.VirtualPath.length());
      return mount.Driver->FileExists(relPath);
    }
  }
  return false;
}

} // namespace Mockingbird::Core
