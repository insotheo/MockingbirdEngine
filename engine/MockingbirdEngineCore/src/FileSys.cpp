#include "Core/FileSys.hpp"

#include "Core/Log.hpp"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace Mockingbird::Core::FileSys {
std::vector<uint8_t> LoadFileBytes(const std::string &filepath) {
  std::ifstream file(filepath, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    ME_LOG_CORE_ERROR("Failed to open file: {}", filepath);
    return {};
  }

  std::streamsize fileSize = file.tellg();

  std::vector<uint8_t> buffer(fileSize);

  file.seekg(0, std::ios::beg);
  if (!file.read(reinterpret_cast<char *>(buffer.data()), fileSize)) {
    ME_LOG_CORE_ERROR("Failed to read file completly: {}", filepath);
    return {};
  }

  return buffer;
}

ImageFile LoadImage(const std::string &filepath) {
  int width, height, channels;

  unsigned char *pixels =
      stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
  if (!pixels) {
    ME_LOG_CORE_ERROR("Failed to load image through STB: {}",
                      stbi_failure_reason());
    return {};
  }

  return ImageFile{
      .Width = width,
      .Height = height,
      .Channels = channels,
      .Pixels = pixels,
  };
}

void FreeImage(ImageFile &img) {
  if (img.Pixels)
    stbi_image_free(img.Pixels);
}
} // namespace Mockingbird::Core::FileSys
