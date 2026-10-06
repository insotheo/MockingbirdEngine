#pragma once

#include <Core/Debug.hpp>
#include <SDL3/SDL.h>
#include <cstdint>
#include <string>
#include <vector>

// TODO: Normal auto layout
namespace Mockingbird::SDL::Graphics {
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
  Mat3,
  Mat4,
};

enum class LayoutType { Vertex, UniformStd140 };

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

  case ShaderDataType::Mat3:
    return 4 * 3 * 3;

  case ShaderDataType::Mat4:
    return 4 * 4 * 4;

  default: {
    ME_ASSERT(false, "Unknown ShaderDataType");
    return 0;
  }
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

  default: {
    ME_ASSERT(false, "Unknown ShaderDataType");
    return SDL_GPU_VERTEXELEMENTFORMAT_INVALID;
  }
  }
}

struct MEBufferAttribute {
  std::string Name;
  ShaderDataType Type;
  uint32_t Size;
  uint32_t Offset;
  bool Normalized;

  MEBufferAttribute() {}

  MEBufferAttribute(ShaderDataType type, const std::string &name,
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

    case ShaderDataType::Mat3:
      return 3 * 3;

    case ShaderDataType::Mat4:
      return 4 * 4;

    default: {
      ME_ASSERT(false, "Unknown ShaderDataType");
      return 0;
    }
    }
  }
};

class BufferLayout {
public:
  BufferLayout() {}

  BufferLayout(const std::initializer_list<MEBufferAttribute> &attribs,
               LayoutType layoutType = LayoutType::Vertex)
      : m_Attributes(attribs) {
    CalculateOffsetsAndStride(layoutType);
  }

  inline uint32_t GetStride() const { return m_Stride; }
  inline const std::vector<MEBufferAttribute> &GetAttributes() const {
    return m_Attributes;
  }

  std::vector<MEBufferAttribute>::iterator begin() {
    return m_Attributes.begin();
  }
  std::vector<MEBufferAttribute>::iterator end() { return m_Attributes.end(); }
  std::vector<MEBufferAttribute>::const_iterator begin() const {
    return m_Attributes.begin();
  }
  std::vector<MEBufferAttribute>::const_iterator end() const {
    return m_Attributes.end();
  }

private:
  void CalculateOffsetsAndStride(LayoutType type) {
    uint32_t offset = 0;
    m_Stride = 0;

    for (auto &attr : m_Attributes) {
      if (type == LayoutType::UniformStd140) {
        uint32_t alignment = 4;
        switch (attr.Type) {
        case ShaderDataType::Float2:
        case ShaderDataType::Int2:
          alignment = 8;
          break;

        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
          alignment = 16;
          break;

        default:
          break;
        }

        uint32_t remainder = offset % alignment;
        if (remainder != 0) {
          offset += (alignment - remainder);
        }
      }
      attr.Offset = offset;
      offset += attr.Size;
    }

    if (type == LayoutType::UniformStd140) {
      uint32_t remainder = offset % 16;
      if (remainder != 0) {
        offset += (16 - remainder);
      }
    }
    m_Stride = offset;
  }

private:
  std::vector<MEBufferAttribute> m_Attributes;
  uint32_t m_Stride = 0;
};

} // namespace Mockingbird::SDL::Graphics
