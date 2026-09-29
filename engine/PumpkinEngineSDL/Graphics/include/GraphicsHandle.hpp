#pragma once

#include <cstdint>

namespace Pumpkin::SDL::Graphics {
struct PEGraphicsHandle {
  uint64_t Id = 0;
  bool Valid = true;

  inline uint32_t GetIndex() const { return (uint32_t)(Id & 0xFFFFFFFF); }
  inline uint32_t GetVersion() const { return (uint32_t)(Id >> 32); }
};

using ShaderHandle = PEGraphicsHandle;
using MeshHandle = PEGraphicsHandle;
using PipelineHandle = uint32_t;

constexpr PEGraphicsHandle PEGraphicsHandleNull =
    PEGraphicsHandle{.Valid = false};

#define CREATE_PE_GRAPHICS_HANDLE(version, index)                              \
  (Pumpkin::SDL::Graphics::PEGraphicsHandle{                                   \
      .Id = ((static_cast<uint64_t>(version) << 32) | index)})

} // namespace Pumpkin::SDL::Graphics
