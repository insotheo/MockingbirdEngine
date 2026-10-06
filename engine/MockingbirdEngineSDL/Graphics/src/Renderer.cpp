#include "Renderer.hpp"
#include "GraphicsHandle.hpp"

#include <Core/Log.hpp>
#include <cstring>

// TODO: MAKE IT BEAUTIFUL

namespace Mockingbird::SDL::Graphics {

void Renderer::Init(SDL_Window *wnd) {
  m_Wnd = wnd;

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

SDL_GPUShader *Renderer::CreateShaderStage(std::span<const std::byte> code,
                                           const ShaderStageDesc &desc,
                                           SDL_GPUShaderStage stage) {
  SDL_GPUShaderCreateInfo info{};
  info.code_size = code.size();
  info.code = reinterpret_cast<const uint8_t *>(code.data());
  info.entrypoint = desc.EntryPoint.empty() ? "main" : desc.EntryPoint.c_str();
  info.stage = stage;
  info.format = SDL_GPU_SHADERFORMAT_SPIRV; // TODO: multi shader type support

  info.num_samplers = desc.SamplerCount;
  info.num_storage_buffers = desc.StorageBufferCount;
  info.num_storage_textures = desc.StorageTextureCount;
  info.num_uniform_buffers = desc.UniformBufferCount;

  return SDL_CreateGPUShader(m_Device, &info);
}

ShaderHandle Renderer::CreateShader(std::span<const std::byte> vert,
                                    std::span<const std::byte> frag,
                                    const ShaderProgramDesc &desc) {
  if (!m_Device)
    return MEGraphicsHandleNull;

  SDL_GPUShader *vertShader =
      CreateShaderStage(vert, desc.Vertex, SDL_GPU_SHADERSTAGE_VERTEX);
  if (!vertShader) {
    ME_LOG_CORE_ERROR("Failed to create Vertex Shader: {}", SDL_GetError());
    return MEGraphicsHandleNull;
  }

  SDL_GPUShader *fragShader =
      CreateShaderStage(frag, desc.Fragment, SDL_GPU_SHADERSTAGE_FRAGMENT);
  if (!fragShader) {
    ME_LOG_CORE_ERROR("Failed to create Fragment Shader: {}", SDL_GetError());
    SDL_ReleaseGPUShader(m_Device, vertShader);
    return MEGraphicsHandleNull;
  }

  ShaderInternal program{
      .VertexShader = vertShader, .FragmentShader = fragShader, .Version = 1};

  const uint32_t index = static_cast<uint32_t>(m_Shaders.size());
  m_Shaders.push_back(program);

  return CREATE_ME_GRAPHICS_HANDLE(program.Version, index);
}

PipelineHandle Renderer::CreateGraphicsPipeline(ShaderHandle shader,
                                                const BufferLayout &layout) {
  ShaderInternal &targetShader = m_Shaders[shader.GetIndex()];

  SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};

  SDL_GPUColorTargetDescription colorTargetDesc{};
  colorTargetDesc.format = SDL_GetGPUSwapchainTextureFormat(m_Device, m_Wnd);

  pipelineInfo.target_info.num_color_targets = 1;
  pipelineInfo.target_info.color_target_descriptions = &colorTargetDesc;

  SDL_GPUVertexBufferDescription bufferDesc{};
  bufferDesc.slot = 0;
  bufferDesc.pitch = layout.GetStride();
  bufferDesc.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;

  std::vector<SDL_GPUVertexAttribute> sdlAttribs;
  uint32_t currLoc = 0;

  // TODO: matrix as vec arrays
  for (const auto &attr : layout) {
    SDL_GPUVertexAttribute sdlAttr{};
    sdlAttr.location = currLoc++;
    sdlAttr.buffer_slot = 0;
    sdlAttr.format = ShaderDataTypeToSDL(attr.Type);
    sdlAttr.offset = attr.Offset;
    sdlAttribs.push_back(sdlAttr);
  }

  SDL_GPUVertexInputState vertexInputState{};
  vertexInputState.num_vertex_buffers = 1;
  vertexInputState.vertex_buffer_descriptions = &bufferDesc;
  vertexInputState.num_vertex_attributes =
      static_cast<uint32_t>(sdlAttribs.size());
  vertexInputState.vertex_attributes = sdlAttribs.data();

  pipelineInfo.vertex_input_state = vertexInputState;
  pipelineInfo.vertex_shader = targetShader.VertexShader;
  pipelineInfo.fragment_shader = targetShader.FragmentShader;
  pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  SDL_GPUGraphicsPipeline *pipeline =
      SDL_CreateGPUGraphicsPipeline(m_Device, &pipelineInfo);

  if (!pipeline) {
    ME_LOG_CORE_ERROR("Failed to create Graphics Pipeline: {}", SDL_GetError());
    return 0;
  }

  m_Pipelines.push_back(pipeline);
  return static_cast<uint32_t>(m_Pipelines.size() - 1);
}

void Renderer::DrawMesh(MeshHandle meshHnd, MaterialHandle matHnd,
                        PipelineHandle pipelineHnd) {
  if (!m_IsDrawing || !m_CurrentRenderPass ||
      meshHnd.GetIndex() >= m_Meshes.size() ||
      pipelineHnd >= m_Pipelines.size())
    return;

  if (matHnd) {
    if (matHnd.GetIndex() >= m_Materials.size() ||
        matHnd.GetVersion() != m_Materials[matHnd.GetIndex()].Version)
      return;
  }

  const MeshInternal &mesh = m_Meshes[meshHnd.GetIndex()];
  const VertexBufferInternal &vb = m_VBs[mesh.VertexBuffer.GetIndex()];
  const IndexBufferInternal &ib = m_IBs[mesh.IndexBuffer.GetIndex()];

  SDL_GPUGraphicsPipeline *pipeline = m_Pipelines[pipelineHnd];

  SDL_BindGPUGraphicsPipeline(m_CurrentRenderPass, pipeline);

  SDL_GPUBufferBinding vertBind{.buffer = vb.GPUBuffer, .offset = 0};
  SDL_BindGPUVertexBuffers(m_CurrentRenderPass, 0, &vertBind, 1);

  SDL_GPUBufferBinding indBind{.buffer = ib.GPUBuffer, .offset = 0};
  SDL_BindGPUIndexBuffer(m_CurrentRenderPass, &indBind, ib.IndexFormat);

  if (matHnd) {
    const MaterialInternal &mat = m_Materials[matHnd.GetIndex()];
    if (!mat.UniformCPUBuffer.empty()) {
      SDL_PushGPUFragmentUniformData(
          m_CurrentCmdBuff, mat.Binding, mat.UniformCPUBuffer.data(),
          static_cast<uint32_t>(mat.UniformCPUBuffer.size()));
    }
  }

  SDL_DrawGPUIndexedPrimitives(m_CurrentRenderPass, ib.Count, 1, 0, 0, 0);
}

void Renderer::StartFrame() {
  m_CurrentCmdBuff = SDL_AcquireGPUCommandBuffer(m_Device);
  if (!m_CurrentCmdBuff)
    return;

  bool success = SDL_AcquireGPUSwapchainTexture(
      m_CurrentCmdBuff, m_Wnd, &m_CurrentTargetTexture, nullptr, nullptr);
  if (!success || !m_CurrentTargetTexture) {
    SDL_CancelGPUCommandBuffer(m_CurrentCmdBuff);
    m_CurrentCmdBuff = nullptr;
    return;
  }

  m_FirstPassInFrame = true;
}

void Renderer::EndFrame() {
  if (m_CurrentCmdBuff) {
    SDL_SubmitGPUCommandBuffer(m_CurrentCmdBuff);
    m_CurrentCmdBuff = nullptr;
  }
  m_CurrentTargetTexture = nullptr;
}

void Renderer::BeginDraw2D() {
  if (m_IsDrawing || !m_CurrentCmdBuff)
    return;

  SDL_GPUColorTargetInfo colorTargetInfo{};
  colorTargetInfo.texture = m_CurrentTargetTexture;
  colorTargetInfo.clear_color = m_ClearColor;
  colorTargetInfo.load_op =
      m_FirstPassInFrame ? SDL_GPU_LOADOP_CLEAR : SDL_GPU_LOADOP_LOAD;
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

  m_CurrentRenderPass =
      SDL_BeginGPURenderPass(m_CurrentCmdBuff, &colorTargetInfo, 1, nullptr);

  m_IsDrawing = true;
  m_FirstPassInFrame = false;
}

void Renderer::EndDraw2D() {
  if (!m_IsDrawing)
    return;

  if (m_CurrentRenderPass) {
    SDL_EndGPURenderPass(m_CurrentRenderPass);
    m_CurrentRenderPass = nullptr;
  }

  m_IsDrawing = false;
}

} // namespace Mockingbird::SDL::Graphics
