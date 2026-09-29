#pragma once

#include <cstdint>
#include <vector>

// TODO: Normal auto layout
namespace Pumpkin::SDL::Graphics {
enum class PEVertexFormat { Float2, Float3, Float4 };

struct PEVertexAttribute {
  uint32_t Location;
  PEVertexFormat Format;
  uint32_t Offset;
};

struct PEVertexLayout {
  uint32_t VertexSize;
  std::vector<PEVertexAttribute> Attributes;
};

} // namespace Pumpkin::SDL::Graphics
