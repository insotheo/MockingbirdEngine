#include "Graphics/Renderer.hpp"
#include "Core/Math.hpp"

#include <Core/Log.hpp>
#include <cstring>

// TODO: MAKE IT BEAUTIFUL

namespace Mockingbird::SDL::Graphics {

void Renderer::Init(SDL_Window *wnd) {
  m_Wnd = wnd;

  SDL_SetHint(SDL_HINT_RENDER_DRIVER, "vulkan");

  m_Device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV,
#if ME_DEBUG
                                 true,
#else
                                 false,
#endif
                                 nullptr);

  if (!m_Device) {
    ME_LOG_CORE_ERROR("Failed to create GPU device: {}", SDL_GetError());
    m_Device = nullptr;
    return;
  }

  if (!SDL_ClaimWindowForGPUDevice(m_Device, m_Wnd)) {
    ME_LOG_CORE_ERROR("Failed to claim window for gpu: {}", SDL_GetError());
    SDL_DestroyGPUDevice(m_Device);
    m_Device = nullptr;
    return;
  }

  SDL_GPUSamplerCreateInfo samplerInfo = {
      .min_filter = SDL_GPU_FILTER_LINEAR,
      .mag_filter = SDL_GPU_FILTER_LINEAR,
      .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
      .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
      .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
  };
  m_DefaultSampler = SDL_CreateGPUSampler(m_Device, &samplerInfo);

  SetVSync(m_VSync);
}

void Renderer::Shutdown() {
  if (!m_Device)
    return;

  SDL_WaitForGPUIdle(m_Device);

  for (auto &pipeline : m_Pipelines) {
    if (pipeline)
      SDL_ReleaseGPUGraphicsPipeline(m_Device, pipeline);
  }
  m_Pipelines.clear();
  m_Materials.clear();

  for (auto &internalShader : m_Shaders) {
    if (internalShader.VertexShader)
      SDL_ReleaseGPUShader(m_Device, internalShader.VertexShader);
    if (internalShader.FragmentShader)
      SDL_ReleaseGPUShader(m_Device, internalShader.FragmentShader);
  }
  m_Shaders.clear();

  for (auto &internalVb : m_VBs) {
    if (internalVb.GPUBuffer)
      SDL_ReleaseGPUBuffer(m_Device, internalVb.GPUBuffer);
  }
  for (auto &internalIb : m_IBs) {
    if (internalIb.GPUBuffer)
      SDL_ReleaseGPUBuffer(m_Device, internalIb.GPUBuffer);
  }
  m_VBs.clear();
  m_IBs.clear();
  m_Meshes.clear();

  for (auto &texture : m_Textures) {
    if (texture.Texture)
      SDL_ReleaseGPUTexture(m_Device, texture.Texture);
  }
  m_Textures.clear();
  if (m_DefaultSampler)
    SDL_ReleaseGPUSampler(m_Device, m_DefaultSampler);

  if (m_Wnd && m_Device)
    SDL_ReleaseWindowFromGPUDevice(m_Device, m_Wnd);

  SDL_DestroyGPUDevice(m_Device);

  m_Device = nullptr;
  m_Wnd = nullptr;
}

void Renderer::SetVSync(bool state) {
  if (!m_Device || !m_Wnd)
    return;

  m_VSync = state;

  SDL_GPUPresentMode presentMode =
      state ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_IMMEDIATE;

  SDL_SetGPUSwapchainParameters(m_Device, m_Wnd,
                                SDL_GPU_SWAPCHAINCOMPOSITION_SDR, presentMode);

  ME_LOG_CORE_INFO("VSync state changed to: {}",
                   (state ? "ENABLED" : "DISABLED"));
}

SDL_GPUBuffer *Renderer::UploadDataToGPU(const void *data, uint32_t byteSize,
                                         SDL_GPUBufferUsageFlags usage) {
  if (!m_Device || !data || byteSize == 0)
    return nullptr;

  SDL_GPUBufferCreateInfo buffInfo{.usage = usage, .size = byteSize};
  SDL_GPUBuffer *gpuBuff = SDL_CreateGPUBuffer(m_Device, &buffInfo);
  if (!gpuBuff)
    return nullptr;

  SDL_GPUTransferBufferCreateInfo transferInfo{
      .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = byteSize};
  SDL_GPUTransferBuffer *transferBuff =
      SDL_CreateGPUTransferBuffer(m_Device, &transferInfo);
  if (!transferBuff) {
    SDL_ReleaseGPUBuffer(m_Device, gpuBuff);
    return nullptr;
  }

  void *mappedData = SDL_MapGPUTransferBuffer(m_Device, transferBuff, false);
  std::memcpy(mappedData, data, byteSize);
  SDL_UnmapGPUTransferBuffer(m_Device, transferBuff);

  SDL_GPUCommandBuffer *cmdBuff = SDL_AcquireGPUCommandBuffer(m_Device);
  if (!cmdBuff) {
    SDL_ReleaseGPUTransferBuffer(m_Device, transferBuff);
    SDL_ReleaseGPUBuffer(m_Device, gpuBuff);
    return nullptr;
  }

  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmdBuff);
  SDL_GPUTransferBufferLocation srcLoc{.transfer_buffer = transferBuff,
                                       .offset = 0};
  SDL_GPUBufferRegion dstRegion{
      .buffer = gpuBuff, .offset = 0, .size = byteSize};
  SDL_UploadToGPUBuffer(copyPass, &srcLoc, &dstRegion, false);
  SDL_EndGPUCopyPass(copyPass);

  SDL_SubmitGPUCommandBuffer(cmdBuff);
  SDL_ReleaseGPUTransferBuffer(m_Device, transferBuff);

  return gpuBuff;
}

VertexBufferHandle Renderer::CreateVertexBuffer(void *vertices,
                                                uint32_t verticesCount,
                                                uint32_t vertexStride) {
  const uint32_t byteSize = verticesCount * vertexStride;
  SDL_GPUBuffer *gpuBuff =
      UploadDataToGPU(vertices, byteSize, SDL_GPU_BUFFERUSAGE_VERTEX);

  if (!gpuBuff) {
    ME_LOG_CORE_ERROR("Failed to create Vertex GPU Buffer for mesh");
    return MEGraphicsHandleNull;
  }

  VertexBufferInternal buff{
      .GPUBuffer = gpuBuff, .Count = verticesCount, .Version = 1};

  const uint32_t index = static_cast<uint32_t>(m_VBs.size());
  m_VBs.push_back(buff);
  return CREATE_ME_GRAPHICS_HANDLE(1, index);
}

IndexBufferHandle Renderer::CreateIndexBuffer(void *indicies,
                                              uint32_t indicesCount,
                                              uint32_t indexStride) {
  const uint32_t byteSize = indicesCount * indexStride;
  SDL_GPUBuffer *gpuBuff =
      UploadDataToGPU(indicies, byteSize, SDL_GPU_BUFFERUSAGE_INDEX);

  if (!gpuBuff) {
    ME_LOG_CORE_ERROR("Failed to create Index GPU Buffer!");
    return MEGraphicsHandleNull;
  }

  IndexBufferInternal buff{.GPUBuffer = gpuBuff,
                           .Count = indicesCount,
                           .IndexFormat = (indexStride == 2)
                                              ? SDL_GPU_INDEXELEMENTSIZE_16BIT
                                              : SDL_GPU_INDEXELEMENTSIZE_32BIT,
                           .Version = 1};

  const uint32_t index = static_cast<uint32_t>(m_IBs.size());
  m_IBs.push_back(buff);
  return CREATE_ME_GRAPHICS_HANDLE(1, index);
}

MeshHandle Renderer::CreateMesh(VertexBufferHandle vertexBuffer,
                                IndexBufferHandle indexBuffer) {
  const uint32_t vbIdx = vertexBuffer.GetIndex();
  const uint32_t indIdx = indexBuffer.GetIndex();

  if (vbIdx >= m_VBs.size() || indIdx >= m_IBs.size()) {
    ME_LOG_CORE_ERROR("Failed to create mesh. Handle index out of range");
    return MEGraphicsHandleNull;
  }

  if (m_VBs[vbIdx].Version != vertexBuffer.GetVersion() ||
      m_IBs[indIdx].Version != indexBuffer.GetVersion()) {
    ME_LOG_CORE_ERROR("Failed to create mesh. Buffer version mismatch");
    return MEGraphicsHandleNull;
  }

  if (!m_VBs[vbIdx].GPUBuffer || !m_IBs[indIdx].GPUBuffer) {
    ME_LOG_CORE_ERROR("Failed to create mesh. GPU buffers not found");
    return MEGraphicsHandleNull;
  }

  MeshInternal mesh{
      .VertexBuffer = vertexBuffer, .IndexBuffer = indexBuffer, .Version = 1};

  const uint32_t index = static_cast<uint32_t>(m_Meshes.size());
  m_Meshes.push_back(mesh);
  return CREATE_ME_GRAPHICS_HANDLE(mesh.Version, index);
}

SDL_GPUShader *Renderer::CreateShaderStage(void *code, size_t codeSize,
                                           const ShaderStageDesc &desc,
                                           SDL_GPUShaderStage stage) {
  SDL_GPUShaderCreateInfo info{};
  info.code_size = codeSize;
  info.code = reinterpret_cast<const uint8_t *>(code);
  info.entrypoint = desc.EntryPoint.empty() ? "main" : desc.EntryPoint.c_str();
  info.stage = stage;
  info.format = SDL_GPU_SHADERFORMAT_SPIRV;

  info.num_samplers = desc.SamplerCount;
  info.num_storage_buffers = desc.StorageBufferCount;
  info.num_storage_textures = desc.StorageTextureCount;
  info.num_uniform_buffers = desc.UniformBufferCount;

  return SDL_CreateGPUShader(m_Device, &info);
}

ShaderHandle Renderer::CreateShader(void *vert, size_t vertSize, void *frag,
                                    size_t fragSize,
                                    const ShaderProgramDesc &desc) {
  if (!m_Device)
    return MEGraphicsHandleNull;

  ShaderProgramDesc autoDesc = desc;

  if (!vert || vertSize == 0 || !frag || fragSize == 0) {
    ME_LOG_CORE_ERROR("Vertex or fragment shader was empty");
    return MEGraphicsHandleNull;
  }

  const uint32_t vertWordCount =
      static_cast<uint32_t>(vertSize / sizeof(uint32_t));
  const uint32_t fragWordCount =
      static_cast<uint32_t>(fragSize / sizeof(uint32_t));

  const uint32_t *vertSpirvBytecode = reinterpret_cast<const uint32_t *>(vert);
  const uint32_t *fragSpirvBytecode = reinterpret_cast<const uint32_t *>(frag);

  spirv_cross::Compiler vertCompiler(vertSpirvBytecode, vertWordCount);
  spirv_cross::Compiler fragCompiler(fragSpirvBytecode, fragWordCount);

  ReflectShaderStage(vertCompiler, autoDesc.Vertex);
  ReflectShaderStage(fragCompiler, autoDesc.Fragment);

  SDL_GPUShader *vertShader = CreateShaderStage(vert, vertSize, autoDesc.Vertex,
                                                SDL_GPU_SHADERSTAGE_VERTEX);
  if (!vertShader) {
    ME_LOG_CORE_ERROR("Failed to create Vertex Shader: {}", SDL_GetError());
    return MEGraphicsHandleNull;
  }

  SDL_GPUShader *fragShader = CreateShaderStage(
      frag, fragSize, autoDesc.Fragment, SDL_GPU_SHADERSTAGE_FRAGMENT);
  if (!fragShader) {
    ME_LOG_CORE_ERROR("Failed to create Fragment Shader: {}", SDL_GetError());
    SDL_ReleaseGPUShader(m_Device, vertShader);
    return MEGraphicsHandleNull;
  }

  std::unordered_map<std::string, UniformBufferInternal> vertexUniforms =
      ReflectUniformBufferLayout(vertCompiler);
  auto fb = vertexUniforms.find("ME_FrameBuffer");
  auto ob = vertexUniforms.find("ME_ObjectBuffer");
  if (fb == vertexUniforms.end() || ob == vertexUniforms.end() ||
      fb->second.Binding != 0 || ob->second.Binding != 1) {
    ME_LOG_CORE_ERROR("Invalid vertex shader. Engine buffers not found or "
                      "placed incorrectly");
    SDL_ReleaseGPUShader(m_Device, fragShader);
    SDL_ReleaseGPUShader(m_Device, vertShader);
    return MEGraphicsHandleNull;
  }
  if (m_FrameBufferUniform.empty() || m_ObjectBufferPropertyOffsets.empty()) {
    m_FrameBufferUniform.resize(
        vertexUniforms["ME_FrameBuffer"].Layout.GetStride(), 0);
    m_ObjectBufferUniform.resize(
        vertexUniforms["ME_ObjectBuffer"].Layout.GetStride(), 0);

    for (const auto &attr : vertexUniforms["ME_FrameBuffer"].Layout) {
      m_FrameBufferPropertyOffsets[attr.Name] = attr.Offset;
    }
    for (const auto &attr : vertexUniforms["ME_ObjectBuffer"].Layout) {
      m_ObjectBufferPropertyOffsets[attr.Name] = attr.Offset;
    }
  }

  std::unordered_map<std::string, UniformBufferInternal> fragmentUniforms =
      ReflectUniformBufferLayout(fragCompiler);

  ShaderInternal program{.VertexShader = vertShader,
                         .FragmentShader = fragShader,
                         .VertexLayout = ReflectVertexLayout(vertCompiler),
                         .VertexUniforms = std::move(vertexUniforms),
                         .FragmentUniforms = std::move(fragmentUniforms),
                         .TexturesCount = autoDesc.Fragment.SamplerCount,
                         .Version = 1};

  const uint32_t index = static_cast<uint32_t>(m_Shaders.size());
  m_Shaders.push_back(program);

  return CREATE_ME_GRAPHICS_HANDLE(program.Version, index);
}

TextureHandle Renderer::CreateTexture(const Core::ImageFile &imgData) {
  if (imgData.Data.empty()) {
    ME_LOG_CORE_ERROR("Cannot create texture. Image is empty.");
    return MEGraphicsHandleNull;
  }

  uint32_t bufferSize = imgData.Width * imgData.Height * 4;

  SDL_GPUTextureCreateInfo textureInfo = {
      .type = SDL_GPU_TEXTURETYPE_2D,
      .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
      .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
      .width = static_cast<uint32_t>(imgData.Width),
      .height = static_cast<uint32_t>(imgData.Height),
      .layer_count_or_depth = 1,
      .num_levels = 1,
  };

  SDL_GPUTexture *gpuTexture = SDL_CreateGPUTexture(m_Device, &textureInfo);
  if (!gpuTexture) {
    ME_LOG_CORE_ERROR("Failed to create GPU Texutre: {}", SDL_GetError());
    return MEGraphicsHandleNull;
  }

  SDL_GPUTransferBufferCreateInfo transferInfo = {
      .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
      .size = bufferSize,
  };
  SDL_GPUTransferBuffer *transferBuff =
      SDL_CreateGPUTransferBuffer(m_Device, &transferInfo);

  void *buffData = SDL_MapGPUTransferBuffer(m_Device, transferBuff, false);
  std::memcpy(buffData, imgData.Data.data(), bufferSize);
  SDL_UnmapGPUTransferBuffer(m_Device, transferBuff);

  SDL_GPUCommandBuffer *cmdBuff = SDL_AcquireGPUCommandBuffer(m_Device);
  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(cmdBuff);

  SDL_GPUTextureTransferInfo srcInfo = {
      .transfer_buffer = transferBuff,
      .offset = 0,
  };
  SDL_GPUTextureRegion dstRegion = {
      .texture = gpuTexture,
      .w = static_cast<uint32_t>(imgData.Width),
      .h = static_cast<uint32_t>(imgData.Height),
      .d = 1,
  };

  SDL_UploadToGPUTexture(copyPass, &srcInfo, &dstRegion, false);
  SDL_EndGPUCopyPass(copyPass);

  SDL_SubmitGPUCommandBuffer(cmdBuff);
  SDL_ReleaseGPUTransferBuffer(m_Device, transferBuff);

  TextureInternal texture = {
      .Texture = gpuTexture,
      .Version = 1,
  };

  const uint32_t index = static_cast<uint32_t>(m_Textures.size());
  m_Textures.push_back(texture);

  return CREATE_ME_GRAPHICS_HANDLE(texture.Version, index);
}

PipelineHandle Renderer::CreateGraphicsPipeline(ShaderHandle shader,
                                                const PipelineStates &states) {
  ShaderInternal &targetShader = m_Shaders[shader.GetIndex()];

  std::vector<SDL_GPUVertexAttribute> sdlAttribs;
  const auto &attributes = targetShader.VertexLayout.GetAttributes();

  for (size_t i = 0; i < attributes.size(); ++i) {
    const auto &attr = attributes[i];
    SDL_GPUVertexAttribute el{};
    el.location = static_cast<uint32_t>(i);
    el.buffer_slot = 0;
    el.format = ShaderDataTypeToSDL(attr.Type);
    el.offset = attr.Offset;

    sdlAttribs.push_back(el);
  }

  SDL_GPUVertexBufferDescription bufferDesc{};
  bufferDesc.slot = 0;
  bufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  bufferDesc.instance_step_rate = 0;
  bufferDesc.pitch = targetShader.VertexLayout.GetStride();

  SDL_GPUVertexInputState vertexInputState{};
  vertexInputState.vertex_buffer_descriptions = &bufferDesc;
  vertexInputState.num_vertex_buffers = 1;
  vertexInputState.vertex_attributes = sdlAttribs.data();
  vertexInputState.num_vertex_attributes =
      static_cast<uint32_t>(sdlAttribs.size());

  SDL_GPUColorTargetDescription colorTargetDesc{};
  colorTargetDesc.format = states.ColorTargetFormat;
  // TODO blending

  SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.vertex_shader = targetShader.VertexShader;
  pipelineInfo.fragment_shader = targetShader.FragmentShader;

  pipelineInfo.vertex_input_state = vertexInputState;

  pipelineInfo.primitive_type = states.PrimitiveType;
  pipelineInfo.rasterizer_state = states.RasterizerState;
  // pipelineInfo.depth_stencil_state = states.DepthStencilState;

  pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;
  pipelineInfo.target_info.num_color_targets = 1;
  // pipelineInfo.target_info.depth_stencil_format = states.DepthStencilFormat;
  // pipelineInfo.target_info.has_depth_stencil_target =
  //     (states.DepthStencilFormat != SDL_GPU_TEXTUREFORMAT_INVALID);

  SDL_GPUGraphicsPipeline *pipeline =
      SDL_CreateGPUGraphicsPipeline(m_Device, &pipelineInfo);

  if (!pipeline) {
    ME_LOG_CORE_ERROR("Failed to create Graphics Pipeline: {}", SDL_GetError());
    return 0;
  }

  m_Pipelines.push_back(pipeline);
  return static_cast<uint32_t>(m_Pipelines.size() - 1);
}

void Renderer::BeginFrame(const Core::MEMat4 &viewProj) {
  m_CurrViewProj = viewProj;
  m_CurrentCmdBuff = SDL_AcquireGPUCommandBuffer(m_Device);
}

void Renderer::ExecuteRenderPass(SDL_GPUTexture *targetTexture,
                                 const Core::MEVec4 &clearColor) {

  if (!m_CurrentCmdBuff || m_RenderQueue.empty())
    return;

  SDL_GPUColorTargetInfo colorTargetInfo{};
  if (targetTexture == nullptr) {
    SDL_AcquireGPUSwapchainTexture(m_CurrentCmdBuff, m_Wnd,
                                   &m_CurrentTargetTexture, nullptr, nullptr);
    colorTargetInfo.texture = m_CurrentTargetTexture;
  } else {
    colorTargetInfo.texture = targetTexture;
  }
  m_CurrentTargetTexture = colorTargetInfo.texture;

  if (!m_CurrentTargetTexture) {
    m_RenderQueue.clear();
    return;
  }

  colorTargetInfo.clear_color =
      SDL_FColor{clearColor.r, clearColor.g, clearColor.b, clearColor.a};
  colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

  std::sort(m_RenderQueue.begin(), m_RenderQueue.end(),
            [](const auto &a, const auto &b) {
              if (a.Pipeline != b.Pipeline)
                return a.Pipeline < b.Pipeline;
              return a.Material.Id < b.Material.Id;
            });

  if (targetTexture == nullptr) {
    for (const auto &layer : m_RenderLayers) {
      if (layer.PreRenderCallback)
        layer.PreRenderCallback(m_CurrentCmdBuff);
    }
  }

  m_CurrentRenderPass =
      SDL_BeginGPURenderPass(m_CurrentCmdBuff, &colorTargetInfo, 1, nullptr);

  SDL_GPUGraphicsPipeline *lastPipeline = nullptr;

  for (const auto &cmd : m_RenderQueue) {
    SDL_GPUGraphicsPipeline *pipeline = m_Pipelines[cmd.Pipeline];

    // bind pipeline
    if (pipeline != lastPipeline) {
      SDL_BindGPUGraphicsPipeline(m_CurrentRenderPass, pipeline);
      lastPipeline = pipeline;
    }

    // bind geometry
    const MeshInternal &mesh = m_Meshes[cmd.Mesh.GetIndex()];
    const VertexBufferInternal &vb = m_VBs[mesh.VertexBuffer.GetIndex()];
    const IndexBufferInternal &ib = m_IBs[mesh.IndexBuffer.GetIndex()];

    SDL_GPUBufferBinding vertBind{.buffer = vb.GPUBuffer, .offset = 0};
    SDL_BindGPUVertexBuffers(m_CurrentRenderPass, 0, &vertBind, 1);
    SDL_GPUBufferBinding indBind{.buffer = ib.GPUBuffer, .offset = 0};
    SDL_BindGPUIndexBuffer(m_CurrentRenderPass, &indBind, ib.IndexFormat);

    // updating ME_FrameBuffer
    UpdateSystemBufferValue(
        m_FrameBufferUniform, m_FrameBufferPropertyOffsets, "uViewProjection",
        Core::Math::ValuePtrMat4(m_CurrViewProj), 4 * 4 * sizeof(float));
    // TODO: Transfer camera position, time as well
    SDL_PushGPUVertexUniformData(
        m_CurrentCmdBuff, 0, m_FrameBufferUniform.data(),
        static_cast<uint32_t>(m_FrameBufferUniform.size()));
    // updating ME_ObjectBuffer
    UpdateSystemBufferValue(
        m_ObjectBufferUniform, m_ObjectBufferPropertyOffsets, "uModel",
        Core::Math::ValuePtrMat4(cmd.Model), 4 * 4 * sizeof(float));
    SDL_PushGPUVertexUniformData(
        m_CurrentCmdBuff, 1, m_ObjectBufferUniform.data(),
        static_cast<uint32_t>(m_ObjectBufferUniform.size()));

    // bind material and textures
    if (cmd.Material) {
      const MaterialInternal &mat = m_Materials[cmd.Material.GetIndex()];
      if (!mat.UniformCPUBuffer.empty()) {
        if (mat.IsFragment) {
          SDL_PushGPUFragmentUniformData(
              m_CurrentCmdBuff, mat.Binding, mat.UniformCPUBuffer.data(),
              static_cast<uint32_t>(mat.UniformCPUBuffer.size()));
        } else {
          SDL_PushGPUVertexUniformData(
              m_CurrentCmdBuff, mat.Binding, mat.UniformCPUBuffer.data(),
              static_cast<uint32_t>(mat.UniformCPUBuffer.size()));
        }
      }

      if (!mat.Textures.empty()) {
        for (size_t i = 0; i < mat.Textures.size(); ++i) {
          const TextureHandle tHnd = mat.Textures[i];
          if (!tHnd)
            continue;

          const TextureInternal &texture = m_Textures[tHnd.GetIndex()];

          SDL_GPUTextureSamplerBinding textSamplBinding{
              .texture = texture.Texture, .sampler = m_DefaultSampler};

          SDL_BindGPUFragmentSamplers(m_CurrentRenderPass,
                                      static_cast<uint32_t>(i),
                                      &textSamplBinding, 1);
        }
      }
    }

    SDL_DrawGPUIndexedPrimitives(m_CurrentRenderPass, ib.Count, 1, 0, 0, 0);
  }

  if (targetTexture == nullptr) {
    for (const auto &layer : m_RenderLayers) {
      if (layer.Callback)
        layer.Callback(m_CurrentRenderPass);
    }
  }

  SDL_EndGPURenderPass(m_CurrentRenderPass);
  m_CurrentRenderPass = nullptr;
  m_RenderQueue.clear();
}

void Renderer::EndFrameAndFlush() {
  if (m_CurrentCmdBuff) {
    SDL_SubmitGPUCommandBuffer(m_CurrentCmdBuff);
  }
  m_CurrentCmdBuff = nullptr;
  m_CurrentTargetTexture = nullptr;
}

} // namespace Mockingbird::SDL::Graphics
