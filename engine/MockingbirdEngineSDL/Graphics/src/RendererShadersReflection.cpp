#include "Renderer.hpp"

#include <Core/Log.hpp>
#include <algorithm>
#include <unordered_map>

namespace Mockingbird::SDL::Graphics {

void Renderer::ReflectShaderStage(const void *code, size_t codeSize,
                                  ShaderStageDesc &outDesc) {
  if (!code || codeSize == 0)
    return;

  const size_t wordCount = codeSize / sizeof(uint32_t);
  const uint32_t *spirvData = reinterpret_cast<const uint32_t *>(code);

  try {
    spirv_cross::Compiler compiler(spirvData, wordCount);

    auto entryPoints = compiler.get_entry_points_and_stages();
    if (!entryPoints.empty()) {
      outDesc.EntryPoint = compiler.get_cleansed_entry_point_name(
          entryPoints[0].name, entryPoints[0].execution_model);
    } else {
      outDesc.EntryPoint = "main";
    }

    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    outDesc.UniformBufferCount =
        static_cast<uint32_t>(resources.uniform_buffers.size());
    outDesc.StorageBufferCount =
        static_cast<uint32_t>(resources.storage_buffers.size());
    outDesc.StorageTextureCount =
        static_cast<uint32_t>(resources.storage_images.size());

    outDesc.SamplerCount = static_cast<uint32_t>(
        resources.sampled_images.size() + resources.separate_samplers.size());

  } catch (const std::exception &e) {
    ME_LOG_CORE_ERROR("SPIRV-Cross reflection failed: {}", e.what());
  }
}

ShaderDataType Renderer::ConvertSPIRVType(const spirv_cross::SPIRType &type) {
  if (type.basetype == spirv_cross::SPIRType::Float) {
    if (type.columns == 1) {
      if (type.vecsize == 1)
        return ShaderDataType::Float;
      if (type.vecsize == 2)
        return ShaderDataType::Float2;
      if (type.vecsize == 3)
        return ShaderDataType::Float3;
      if (type.vecsize == 4)
        return ShaderDataType::Float4;
    } else if (type.columns == 3 && type.vecsize == 3) {
      return ShaderDataType::Mat3;
    } else if (type.columns == 4 && type.vecsize == 4) {
      return ShaderDataType::Mat4;
    }
  } else if (type.basetype == spirv_cross::SPIRType::Int) {
    if (type.vecsize == 1)
      return ShaderDataType::Int;
    if (type.vecsize == 2)
      return ShaderDataType::Int2;
    if (type.vecsize == 3)
      return ShaderDataType::Int3;
    if (type.vecsize == 4)
      return ShaderDataType::Int4;
  }
  return ShaderDataType::None;
}

std::unordered_map<std::string, Renderer::UniformBufferInternal>
Renderer::ReflectUniformBufferLayout(const void *spirvBytecode,
                                     size_t bytecodeSize) {
  const uint32_t wordCount = bytecodeSize / sizeof(uint32_t);
  const uint32_t *bytecode = reinterpret_cast<const uint32_t *>(spirvBytecode);

  spirv_cross::Compiler compiler(bytecode, wordCount);
  spirv_cross::ShaderResources resources = compiler.get_shader_resources();

  std::unordered_map<std::string, UniformBufferInternal> uniforms;

  for (auto &ubo : resources.uniform_buffers) {
    std::string uboName = ubo.name;
    const uint32_t binding =
        compiler.get_decoration(ubo.id, spv::DecorationBinding);

    const spirv_cross::SPIRType &uboType = compiler.get_type(ubo.type_id);

    std::vector<MEBufferAttribute> attributes;
    uint32_t memberCount = static_cast<uint32_t>(uboType.member_types.size());

    for (uint32_t i = 0; i < memberCount; ++i) {
      const auto &memberType = compiler.get_type(uboType.member_types[i]);

      std::string memberName = compiler.get_member_name(ubo.base_type_id, i);
      ShaderDataType sdt = ConvertSPIRVType(memberType);

      uint32_t offset = compiler.type_struct_member_offset(uboType, i);
      size_t size = compiler.get_declared_struct_member_size(uboType, i);

      MEBufferAttribute attr(sdt, memberName, false);
      attr.Offset = offset;
      attr.Size = static_cast<uint32_t>(size);

      attributes.push_back(attr);
    }

    uint32_t totalBuffSize =
        static_cast<uint32_t>(compiler.get_declared_struct_size(uboType));

    UniformBufferInternal buff{};
    buff.Layout = BufferLayout(attributes, totalBuffSize);
    buff.Binding = binding;

    uniforms[uboName] = std::move(buff);
  }

  return uniforms;
}

BufferLayout Renderer::ReflectVertexLayout(const void *spirvBytecode,
                                           size_t bytecodeSize) {
  const uint32_t wordCount = bytecodeSize / sizeof(uint32_t);
  const uint32_t *bytecode = reinterpret_cast<const uint32_t *>(spirvBytecode);

  spirv_cross::Compiler compiler(bytecode, wordCount);
  spirv_cross::ShaderResources resources = compiler.get_shader_resources();

  struct SortedAttribute {
    uint32_t location;
    MEBufferAttribute attribute;
  };
  std::vector<SortedAttribute> sortedAttribs;

  for (const auto &resource : resources.stage_inputs) {
    uint32_t location =
        compiler.get_decoration(resource.id, spv::DecorationLocation);
    const auto &type = compiler.get_type(resource.type_id);

    ShaderDataType sdt = ConvertSPIRVType(type);
    std::string name = compiler.get_name(resource.id);

    MEBufferAttribute attr(sdt, name, false);
    sortedAttribs.push_back({location, attr});
  }

  std::sort(sortedAttribs.begin(), sortedAttribs.end(),
            [](const SortedAttribute &a, const SortedAttribute &b) {
              return a.location < b.location;
            });

  std::vector<MEBufferAttribute> finalAttributes;
  for (const auto &item : sortedAttribs) {
    finalAttributes.push_back(item.attribute);
  }

  return BufferLayout(finalAttributes, LayoutType::Vertex);
}

} // namespace Mockingbird::SDL::Graphics
