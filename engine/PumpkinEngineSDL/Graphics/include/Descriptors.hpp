#pragma once

#include <cstdint>

namespace Pumpkin::SDL::Graphics {

struct ShaderStageDesc {
  uint32_t SamplerCount = 0;
  uint32_t StorageBufferCount = 0;
  uint32_t StorageTextureCount = 0;
  uint32_t UniformBufferCount = 0;
  const char *EntryPoint = "main";
};

struct ShaderProgramDesc {
  ShaderStageDesc Vertex{};
  ShaderStageDesc Fragment{};
};

} // namespace Pumpkin::SDL::Graphics
