#include "Renderer.hpp"

#include <Core/Log.hpp>
#include <cstring>

// TODO: MAKE IT BEAUTIFUL

namespace Pumpkin::SDL::Graphics {

void Renderer::Init(SDL_Window *wnd) {
  m_Wnd = wnd;

  m_Device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV,
#if PE_DEBUG
                                 true,
#else
                                 false,
#endif
                                 nullptr);

  if (!m_Device) {
    PE_LOG_CORE_ERROR("Failed to create GPU device: {}", SDL_GetError());
    m_Device = nullptr;
    return;
  }

  if (!SDL_ClaimWindowForGPUDevice(m_Device, m_Wnd)) {
    PE_LOG_CORE_ERROR("Failed to claim window for gpu: {}", SDL_GetError());
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

  for (auto &internalShader : m_Shaders) {
    if (internalShader.VertexShader)
      SDL_ReleaseGPUShader(m_Device, internalShader.VertexShader);
    if (internalShader.FragmentShader)
      SDL_ReleaseGPUShader(m_Device, internalShader.FragmentShader);
  }
  m_Shaders.clear();

  for (auto &internalMesh : m_Meshes) {
    if (internalMesh.VertexBuffer)
      SDL_ReleaseGPUBuffer(m_Device, internalMesh.VertexBuffer);
    if (internalMesh.IndexBuffer)
      SDL_ReleaseGPUBuffer(m_Device, internalMesh.IndexBuffer);
  }
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

  PE_LOG_CORE_INFO("VSync state changed to: {}",
                   (state ? "ENABLED" : "DISABLED"));
}

MeshHandle Renderer::CreateMesh(std::span<const std::byte> verticies,
                                uint32_t vertexStride,
                                std::span<const std::byte> indicies,
                                uint32_t indexStride) {
  if (!m_Device || verticies.empty() || indicies.empty()) {
    return PEGraphicsHandleNull;
  }

  const uint32_t vertexByteSize = static_cast<uint32_t>(verticies.size());
  const uint32_t indexByteSize = static_cast<uint32_t>(indicies.size());

  SDL_GPUBufferCreateInfo vertBuffInfo{};
  vertBuffInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  vertBuffInfo.size = vertexByteSize;
  SDL_GPUBuffer *vertBuff = SDL_CreateGPUBuffer(m_Device, &vertBuffInfo);

  SDL_GPUBufferCreateInfo indBuffInfo{};
  indBuffInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
  indBuffInfo.size = indexByteSize;
  SDL_GPUBuffer *indBuff = SDL_CreateGPUBuffer(m_Device, &indBuffInfo);

  if (!vertBuff || !indBuff) {
    PE_LOG_CORE_ERROR("Failed to create GPU Buffers for mesh: {}",
                      SDL_GetError());
    if (vertBuff)
      SDL_ReleaseGPUBuffer(m_Device, vertBuff);
    if (indBuff)
      SDL_ReleaseGPUBuffer(m_Device, indBuff);
    return PEGraphicsHandleNull;
  }

  const uint32_t alignedVertexByteSize = (vertexByteSize + 3) & ~3;
  const uint32_t totalTransferSize = alignedVertexByteSize + indexByteSize;

  SDL_GPUTransferBufferCreateInfo transferInfo{};
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  transferInfo.size = totalTransferSize;
  SDL_GPUTransferBuffer *transferBuff =
      SDL_CreateGPUTransferBuffer(m_Device, &transferInfo);

  if (!transferBuff) {
    PE_LOG_CORE_ERROR("Failed to create Transfer Buffer for mesh: {}",
                      SDL_GetError());
    SDL_ReleaseGPUBuffer(m_Device, vertBuff);
    SDL_ReleaseGPUBuffer(m_Device, indBuff);
    return PEGraphicsHandleNull;
  }

  std::byte *mappedData = reinterpret_cast<std::byte *>(
      SDL_MapGPUTransferBuffer(m_Device, transferBuff, false));
  std::memcpy(mappedData, verticies.data(), vertexByteSize);
  std::memcpy(mappedData + alignedVertexByteSize, indicies.data(),
              indexByteSize);
  SDL_UnmapGPUTransferBuffer(m_Device, transferBuff);

  SDL_GPUCommandBuffer *uploadCmdBuff = SDL_AcquireGPUCommandBuffer(m_Device);
  if (!uploadCmdBuff) {
    PE_LOG_CORE_ERROR("Failed to acquire Command Buffer for mesh: {}",
                      SDL_GetError());
    SDL_ReleaseGPUTransferBuffer(m_Device, transferBuff);
    SDL_ReleaseGPUBuffer(m_Device, vertBuff);
    SDL_ReleaseGPUBuffer(m_Device, indBuff);
    return PEGraphicsHandleNull;
  }

  SDL_GPUCopyPass *copyPass = SDL_BeginGPUCopyPass(uploadCmdBuff);

  SDL_GPUTransferBufferLocation srcLoc{};
  srcLoc.transfer_buffer = transferBuff;

  SDL_GPUBufferRegion dstVertRegion{
      .buffer = vertBuff, .offset = 0, .size = vertexByteSize};
  srcLoc.offset = 0;
  SDL_UploadToGPUBuffer(copyPass, &srcLoc, &dstVertRegion, false);

  SDL_GPUBufferRegion dstIndRegion{
      .buffer = indBuff, .offset = 0, .size = indexByteSize};
  srcLoc.offset = alignedVertexByteSize;
  SDL_UploadToGPUBuffer(copyPass, &srcLoc, &dstIndRegion, false);

  SDL_EndGPUCopyPass(copyPass);

  SDL_SubmitGPUCommandBuffer(uploadCmdBuff);
  SDL_ReleaseGPUTransferBuffer(m_Device, transferBuff);

  MeshInternal mesh;
  mesh.VertexBuffer = vertBuff;
  mesh.IndexBuffer = indBuff;
  mesh.VertexCount = vertexByteSize / vertexStride;
  mesh.IndexCount = indexByteSize / indexStride;
  mesh.IndexFormat = (indexStride == 2) ? SDL_GPU_INDEXELEMENTSIZE_16BIT
                                        : SDL_GPU_INDEXELEMENTSIZE_32BIT;
  mesh.Version = 1;

  const uint32_t index = static_cast<uint32_t>(m_Meshes.size());
  m_Meshes.push_back(mesh);

  return CREATE_PE_GRAPHICS_HANDLE(mesh.Version, index);
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
    return PEGraphicsHandleNull;

  SDL_GPUShader *vertShader =
      CreateShaderStage(vert, desc.Vertex, SDL_GPU_SHADERSTAGE_VERTEX);
  if (!vertShader) {
    PE_LOG_CORE_ERROR("Failed to create Vertex Shader: {}", SDL_GetError());
    return PEGraphicsHandleNull;
  }

  SDL_GPUShader *fragShader =
      CreateShaderStage(frag, desc.Fragment, SDL_GPU_SHADERSTAGE_FRAGMENT);
  if (!fragShader) {
    PE_LOG_CORE_ERROR("Failed to create Fragment Shader: {}", SDL_GetError());
    SDL_ReleaseGPUShader(m_Device, vertShader);
    return PEGraphicsHandleNull;
  }

  ShaderInternal program{
      .VertexShader = vertShader, .FragmentShader = fragShader, .Version = 1};

  const uint32_t index = static_cast<uint32_t>(m_Shaders.size());
  m_Shaders.push_back(program);

  return CREATE_PE_GRAPHICS_HANDLE(program.Version, index);
}

PipelineHandle Renderer::CreateGraphicsPipeline(const ShaderHandle &shader,
                                                const VertexLayout &layout) {
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
    PE_LOG_CORE_ERROR("Failed to create Graphics Pipeline: {}", SDL_GetError());
    return 0;
  }

  m_Pipelines.push_back(pipeline);
  return static_cast<uint32_t>(m_Pipelines.size() - 1);
}

void Renderer::DrawMesh(const MeshHandle &meshHnd, PipelineHandle pipelineHnd) {
  if (!m_IsDrawing || !m_CurrentRenderPass ||
      meshHnd.GetIndex() >= m_Meshes.size() ||
      pipelineHnd >= m_Pipelines.size())
    return;

  const MeshInternal &mesh = m_Meshes[meshHnd.GetIndex()];
  SDL_GPUGraphicsPipeline *pipeline = m_Pipelines[pipelineHnd];

  SDL_BindGPUGraphicsPipeline(m_CurrentRenderPass, pipeline);

  SDL_GPUBufferBinding vertBind{.buffer = mesh.VertexBuffer, .offset = 0};
  SDL_BindGPUVertexBuffers(m_CurrentRenderPass, 0, &vertBind, 1);

  SDL_GPUBufferBinding indBind{.buffer = mesh.IndexBuffer, .offset = 0};
  SDL_BindGPUIndexBuffer(m_CurrentRenderPass, &indBind, mesh.IndexFormat);

  SDL_DrawGPUIndexedPrimitives(m_CurrentRenderPass, mesh.IndexCount, 1, 0, 0,
                               0);
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

} // namespace Pumpkin::SDL::Graphics
