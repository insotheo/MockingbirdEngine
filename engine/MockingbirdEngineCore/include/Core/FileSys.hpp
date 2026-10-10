#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Mockingbird::Core::FileSys {
struct ImageFile {
  int Width = 0;
  int Height = 0;
  int Channels = 0;
  unsigned char *Pixels = nullptr;
};

std::vector<uint8_t> LoadFileBytes(const std::string &filepath);

ImageFile LoadImage(const std::string &filepath);
void FreeImage(ImageFile &img);
} // namespace Mockingbird::Core::FileSys
