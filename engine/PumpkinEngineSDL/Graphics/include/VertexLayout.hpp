#pragma once

#include <SDL3/SDL.h>
#include <cstdint>
#include <string>
#include <vector>

// TODO: Normal auto layout
namespace Pumpkin::SDL::Graphics {
enum class ShaderDataType {
  None = 0,
  Float,
  Float2,
  Float3,
  Float4,
  Int,
  Int2,
  Int3,
  Int4,
};

static uint32_t ShaderDataTypeSize(ShaderDataType t) {
  switch (t) {
  case ShaderDataType::Float:
    return 4;
  case ShaderDataType::Float2:
    return 4 * 2;
  case ShaderDataType::Float3:
    return 4 * 3;
  case ShaderDataType::Float4:
    return 4 * 4;

  case ShaderDataType::Int:
    return 4;
  case ShaderDataType::Int2:
    return 4 * 2;
  case ShaderDataType::Int3:
    return 4 * 3;
  case ShaderDataType::Int4:
    return 4 * 4;

  default:
    return 0;
  }
}

static SDL_GPUVertexElementFormat ShaderDataTypeToSDL(ShaderDataType t) {
  switch (t) {
  case ShaderDataType::Float:
    return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT;
  case ShaderDataType::Float2:
    return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
  case ShaderDataType::Float3:
    return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3;
  case ShaderDataType::Float4:
    return SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;

  case ShaderDataType::Int:
    return SDL_GPU_VERTEXELEMENTFORMAT_INT;
  case ShaderDataType::Int2:
    return SDL_GPU_VERTEXELEMENTFORMAT_INT2;
  case ShaderDataType::Int3:
    return SDL_GPU_VERTEXELEMENTFORMAT_INT3;
  case ShaderDataType::Int4:
    return SDL_GPU_VERTEXELEMENTFORMAT_INT4;

  default:
    return SDL_GPU_VERTEXELEMENTFORMAT_INVALID;
  }
}

struct PEVertexAttribute {
  std::string Name;
  ShaderDataType Type;
  uint32_t Size;
  uint32_t Offset;
  bool Normalized;

  PEVertexAttribute() {}

  PEVertexAttribute(ShaderDataType type, const std::string &name,
                    bool normalized = false)
      : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0),
        Normalized(normalized) {}

  uint32_t GetComponentCount() const {
    switch (Type) {
    case ShaderDataType::Float:
      return 1;
    case ShaderDataType::Float2:
      return 2;
    case ShaderDataType::Float3:
      return 3;
    case ShaderDataType::Float4:
      return 4;

    case ShaderDataType::Int:
      return 1;
    case ShaderDataType::Int2:
      return 2;
    case ShaderDataType::Int3:
      return 3;
    case ShaderDataType::Int4:
      return 4;

    default:
      return 0;
    }
  }
};

class VertexLayout {
public:
  VertexLayout() {}

  VertexLayout(const std::initializer_list<PEVertexAttribute> &attribs)
      : m_Attributes(attribs) {
    CalculateOffsetsAndStride();
  }

  inline uint32_t GetStride() const { return m_Stride; }
  inline const std::vector<PEVertexAttribute> &GetAttributes() const {
    return m_Attributes;
  }

  std::vector<PEVertexAttribute>::iterator begin() {
    return m_Attributes.begin();
  }
  std::vector<PEVertexAttribute>::iterator end() { return m_Attributes.end(); }
  std::vector<PEVertexAttribute>::const_iterator begin() const {
    return m_Attributes.begin();
  }
  std::vector<PEVertexAttribute>::const_iterator end() const {
    return m_Attributes.end();
  }

private:
  void CalculateOffsetsAndStride() {
    uint32_t offset = 0;
    m_Stride = 0;
    for (auto &attr : m_Attributes) {
      attr.Offset = offset;
      offset += attr.Size;
      m_Stride += attr.Size;
    }
  }

private:
  std::vector<PEVertexAttribute> m_Attributes;
  uint32_t m_Stride = 0;
};

} // namespace Pumpkin::SDL::Graphics
