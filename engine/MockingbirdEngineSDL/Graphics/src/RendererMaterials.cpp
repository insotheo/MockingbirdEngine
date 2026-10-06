#include "Renderer.hpp"

#include <cstring>

namespace Mockingbird::SDL::Graphics {

MaterialHandle Renderer::CreateMaterial(PipelineHandle pipelineHnd,
                                        const BufferLayout &layout,
                                        uint32_t binding) {
  if (pipelineHnd < 0 || pipelineHnd >= m_Pipelines.size() ||
      !m_Pipelines[pipelineHnd])
    return MEGraphicsHandleNull;

  MaterialInternal mat;
  mat.PipelineHnd = pipelineHnd;
  mat.UniformCPUBuffer.resize(layout.GetStride(), 0);
  mat.Binding = binding;
  mat.Version = 1;

  for (const auto &attr : layout) {
    mat.PropertyOffsets[attr.Name] = attr.Offset;
  }

  const uint32_t index = static_cast<uint32_t>(m_Materials.size());
  m_Materials.push_back(mat);

  return CREATE_ME_GRAPHICS_HANDLE(mat.Version, index);
}

void Renderer::MaterialSetFloat(MaterialHandle hnd, const std::string &property,
                                float val) {
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, &val, sizeof(float));
  }
}

void Renderer::MaterialSetFloat3(MaterialHandle hnd,
                                 const std::string &property,
                                 const Core::MEVec3 &val) {
  MaterialInternal &mat = m_Materials[hnd.GetIndex()];
  auto it = mat.PropertyOffsets.find(property);
  if (it != mat.PropertyOffsets.end()) {
    float data[3] = {val.x, val.y, val.z};
    std::memcpy(mat.UniformCPUBuffer.data() + it->second, data, sizeof(data));
  }
}

} // namespace Mockingbird::SDL::Graphics
