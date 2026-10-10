#include "Core/Math.hpp"
#include "Renderer.hpp"

#include <Core/Log.hpp>
#include <cstring>

namespace Mockingbird::SDL::Graphics {

// set = 1 - Vertex Material
// set = 2 - Fragment Textures
// set = 3 - Fragment Material(std140)

MaterialHandle Renderer::CreateMaterial(ShaderHandle shaderHnd,
                                        const std::string &bufferName,
                                        bool isFragment) {
  if (shaderHnd.GetIndex() >= m_Shaders.size() ||
      shaderHnd.GetVersion() != m_Shaders[shaderHnd.GetIndex()].Version)
    return MEGraphicsHandleNull;

  ShaderInternal &shader = m_Shaders[shaderHnd.GetIndex()];
  auto &uniformsMap =
      (isFragment ? shader.FragmentUniforms : shader.VertexUniforms);
  if (!uniformsMap.contains(bufferName)) {
    ME_LOG_CORE_ERROR("No buffer {} found at {} shader!", bufferName,
                      (isFragment ? "fragment" : "vertex"));
    return MEGraphicsHandleNull;
  }
  UniformBufferInternal &ub = uniformsMap[bufferName];

  MaterialInternal mat;
  mat.IsFragment = isFragment;
  mat.ShaderHnd = shaderHnd;
  mat.UniformCPUBuffer.resize(ub.Layout.GetStride(), 0);
  mat.Binding = ub.Binding;
  mat.Version = 1;

  for (const auto &attr : ub.Layout) {
    mat.PropertyOffsets[attr.Name] = attr.Offset;
  }

  const uint32_t index = static_cast<uint32_t>(m_Materials.size());
  m_Materials.push_back(mat);

  return CREATE_ME_GRAPHICS_HANDLE(mat.Version, index);
}

void Renderer::MaterialSetFloat(MaterialHandle hnd, const std::string &property,
                                float val) {
  if (!hnd)
    return;
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, &val, sizeof(float));
  }
}

void Renderer::MaterialSetFloat2(MaterialHandle hnd,
                                 const std::string &property,
                                 const Core::MEVec2 &val) {
  if (!hnd)
    return;
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    float data[2] = {val.x, val.y};
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, data, sizeof(data));
  }
}

void Renderer::MaterialSetFloat3(MaterialHandle hnd,
                                 const std::string &property,
                                 const Core::MEVec3 &val) {
  if (!hnd)
    return;
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    float data[3] = {val.x, val.y, val.z};
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, data, sizeof(data));
  }
}

void Renderer::MaterialSetFloat4(MaterialHandle hnd,
                                 const std::string &property,
                                 const Core::MEVec4 &val) {
  if (!hnd)
    return;
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    float data[4] = {val.x, val.y, val.z, val.w};
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, data, sizeof(data));
  }
}

void Renderer::MaterialSetInt(MaterialHandle hnd, const std::string &property,
                              int val) {
  if (!hnd)
    return;
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, &val, sizeof(int));
  }
}

void Renderer::MaterialSetInt2(MaterialHandle hnd, const std::string &property,
                               const Core::MEVec2i &val) {
  if (!hnd)
    return;
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    int data[2] = {val.x, val.y};
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, data, sizeof(data));
  }
}

void Renderer::MaterialSetInt3(MaterialHandle hnd, const std::string &property,
                               const Core::MEVec3i &val) {
  if (!hnd)
    return;
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    int data[3] = {val.x, val.y, val.z};
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, data, sizeof(data));
  }
}

void Renderer::MaterialSetInt4(MaterialHandle hnd, const std::string &property,
                               const Core::MEVec4i &val) {
  if (!hnd)
    return;
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    int data[4] = {val.x, val.y, val.z, val.w};
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, data, sizeof(data));
  }
}

void Renderer::MaterialSetMat3(MaterialHandle hnd, const std::string &property,
                               const Core::MEMat3 &val) {
  if (!hnd)
    return;
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    const float *ptr = Core::Math::ValuePtrMat3(val);
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, ptr,
                3 * 3 * sizeof(float));
  }
}

void Renderer::MaterialSetMat4(MaterialHandle hnd, const std::string &property,
                               const Core::MEMat4 &val) {
  if (!hnd)
    return;
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    const float *ptr = Core::Math::ValuePtrMat4(val);
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, ptr,
                4 * 4 * sizeof(float));
  }
}

} // namespace Mockingbird::SDL::Graphics
