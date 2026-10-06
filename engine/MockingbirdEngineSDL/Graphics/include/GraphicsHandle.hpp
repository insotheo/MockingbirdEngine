#pragma once

#include <cstdint>

namespace Mockingbird::SDL::Graphics {
struct MEGraphicsHandle {
  uint64_t Id = 0;

  inline uint32_t GetIndex() const { return (uint32_t)(Id & 0xFFFFFFFF); }
  inline uint32_t GetVersion() const { return (uint32_t)(Id >> 32); }

  explicit operator bool() const noexcept { return Id != 0; }
};

using ShaderHandle = MEGraphicsHandle;
using MaterialHandle = MEGraphicsHandle;
using MeshHandle = MEGraphicsHandle;
using VertexBufferHandle = MEGraphicsHandle;
using IndexBufferHandle = MEGraphicsHandle;
using PipelineHandle = uint32_t;

constexpr MEGraphicsHandle MEGraphicsHandleNull = MEGraphicsHandle{.Id = 0};

#define CREATE_ME_GRAPHICS_HANDLE(version, index)                              \
  (Mockingbird::SDL::Graphics::MEGraphicsHandle{                               \
      .Id = ((static_cast<uint64_t>(version) << 32) | index)})

} // namespace Mockingbird::SDL::Graphics
