#pragma once

#include "BufferLayout.hpp"
#include "Descriptors.hpp"
#include "GraphicsHandle.hpp"
#include <Core/Math.hpp>
#include <SDL3/SDL.h>
#include <VFS/VFSInfos.hpp>
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <spirv_cross.hpp>
#include <unordered_map>
#include <vector>

namespace Mockingbird::SDL::Graphics {
class Renderer {
public:
  Renderer() {}
  ~Renderer() { Shutdown(); }

  inline bool IsCreatedSuccessfully() const { return m_Device; }

  // For third-party api's! Not recomended to call in engine
  inline SDL_GPUDevice *GetGPU() const { return m_Device; }
  inline SDL_GPUCommandBuffer *GetGPUCurrentCommandBuffer() const {
    return m_CurrentCmdBuff;
  }
  inline SDL_GPURenderPass *GetGPUCurrentRenderPass() const {
    return m_CurrentRenderPass;
  }

  void Init(SDL_Window *wnd);
  void Shutdown();

  void SetVSync(bool state);
  inline bool IsVSyncEnabled() const { return m_VSync; }

  // queue
  inline void RegisterRenderLayer(
      uint32_t priority, const std::string &name,
      std::function<void(SDL_GPUCommandBuffer *)> preRenderCallback,
      std::function<void(SDL_GPURenderPass *)> callback) {
    m_RenderLayers.push_back({priority, preRenderCallback, callback, name});
    std::sort(
        m_RenderLayers.begin(), m_RenderLayers.end(),
        [](const auto &a, const auto &b) { return a.Priority < b.Priority; });
  }

  inline void SubmitMesh(MeshHandle meshHnd, PipelineHandle pipelineHnd,
                         MaterialHandle matHnd) {
    if (meshHnd.GetIndex() >= m_Meshes.size() ||
        pipelineHnd >= m_Pipelines.size() ||
        matHnd.GetIndex() >= m_Materials.size())
      return;

    if (meshHnd.GetVersion() != m_Meshes[meshHnd.GetIndex()].Version ||
        matHnd.GetVersion() != m_Materials[matHnd.GetIndex()].Version)
      return;

    m_RenderCommands.push_back({meshHnd, pipelineHnd, matHnd});
  }

  void BeginFrame();
  void EndFrameAndFlush();

  VertexBufferHandle CreateVertexBuffer(void *vertices, uint32_t verticesCount,
                                        uint32_t vertexStride);

  IndexBufferHandle CreateIndexBuffer(void *indicies, uint32_t indicesCount,
                                      uint32_t indexStride);

  MeshHandle CreateMesh(VertexBufferHandle vertexBuffer,
                        IndexBufferHandle indexBuffer);

  ShaderHandle CreateShader(void *vert, size_t vertSize, void *frag,
                            size_t fragSize,
                            const ShaderProgramDesc &desc = {});

  PipelineHandle CreateGraphicsPipeline(ShaderHandle shader,
                                        const PipelineStates &states = {});

  MaterialHandle CreateMaterial(ShaderHandle shaderHnd,
                                const std::string &bufferName,
                                bool isFragment = true);

  TextureHandle CreateTexture(const Core::ImageFile &imgData);

  inline void SetClearColor(float r, float g, float b, float alpha = 1.0f) {
    m_ClearColor = SDL_FColor{r, g, b, alpha};
  }

  // material
  void MaterialSetFloat(MaterialHandle hnd, const std::string &property,
                        float val);
  void MaterialSetFloat2(MaterialHandle hnd, const std::string &property,
                         const Core::MEVec2 &val);
  void MaterialSetFloat3(MaterialHandle hnd, const std::string &property,
                         const Core::MEVec3 &val);
  void MaterialSetFloat4(MaterialHandle hnd, const std::string &property,
                         const Core::MEVec4 &val);
  void MaterialSetInt(MaterialHandle hnd, const std::string &property, int val);
  void MaterialSetInt2(MaterialHandle hnd, const std::string &property,
                       const Core::MEVec2i &val);
  void MaterialSetInt3(MaterialHandle hnd, const std::string &property,
                       const Core::MEVec3i &val);
  void MaterialSetInt4(MaterialHandle hnd, const std::string &property,
                       const Core::MEVec4i &val);
  void MaterialSetMat3(MaterialHandle hnd, const std::string &property,
                       const Core::MEMat3 &val);
  void MaterialSetMat4(MaterialHandle hnd, const std::string &property,
                       const Core::MEMat4 &val);
  void MaterialSetTexture(MaterialHandle hnd, size_t index,
                          TextureHandle texture);

  // shader getters
  inline const BufferLayout *GetVertexLayout(ShaderHandle shader) const {
    if (shader.GetIndex() >= m_Shaders.size() ||
        shader.GetVersion() != m_Shaders[shader.GetIndex()].Version) {
      return nullptr;
    }
    return &m_Shaders[shader.GetIndex()].VertexLayout;
  }

private:
  struct RenderLayer {
    uint32_t Priority;
    std::function<void(SDL_GPUCommandBuffer *)> PreRenderCallback;
    std::function<void(SDL_GPURenderPass *)> Callback;
    std::string Name;
  };

  struct RenderCommand {
    MeshHandle Mesh;
    PipelineHandle Pipeline;
    MaterialHandle Material;
  };

  struct TextureInternal {
    SDL_GPUTexture *Texture;
    uint32_t Version;
  };

  struct VertexBufferInternal {
    SDL_GPUBuffer *GPUBuffer = nullptr;
    uint32_t Count;
    uint32_t Version;
  };

  struct IndexBufferInternal {
    SDL_GPUBuffer *GPUBuffer = nullptr;
    uint32_t Count;
    SDL_GPUIndexElementSize IndexFormat;
    uint32_t Version;
  };

  struct MeshInternal {
    VertexBufferHandle VertexBuffer;
    IndexBufferHandle IndexBuffer;
    uint32_t Version;
  };

  struct UniformBufferInternal {
    BufferLayout Layout;
    uint32_t Binding;
  };

  struct ShaderInternal {
    SDL_GPUShader *VertexShader;
    SDL_GPUShader *FragmentShader;

    BufferLayout VertexLayout;

    std::unordered_map<std::string, UniformBufferInternal> VertexUniforms;
    std::unordered_map<std::string, UniformBufferInternal> FragmentUniforms;

    uint32_t TexturesCount;

    uint32_t Version = 1;
  };

  struct MaterialInternal {
    std::vector<TextureHandle> Textures;

    bool IsFragment;

    std::vector<uint8_t> UniformCPUBuffer;
    std::unordered_map<std::string, uint32_t> PropertyOffsets;

    uint32_t Version;
    uint32_t Binding = 0;
  };

private:
  // Shader reflection
  void ReflectShaderStage(spirv_cross::Compiler &compiler,
                          ShaderStageDesc &outDesc);

  ShaderDataType ConvertSPIRVType(const spirv_cross::SPIRType &type);

  std::unordered_map<std::string, UniformBufferInternal>
  ReflectUniformBufferLayout(spirv_cross::Compiler &compiler);

  BufferLayout ReflectVertexLayout(spirv_cross::Compiler &compiler);

  SDL_GPUShader *CreateShaderStage(void *code, size_t codeSize,
                                   const ShaderStageDesc &desc,
                                   SDL_GPUShaderStage stage);

  SDL_GPUBuffer *UploadDataToGPU(const void *data, uint32_t byteSize,
                                 SDL_GPUBufferUsageFlags usage);

private:
  SDL_GPUDevice *m_Device = nullptr;
  SDL_Window *m_Wnd = nullptr;

  SDL_FColor m_ClearColor = SDL_FColor{0.1f, 0.1f, 0.1f, 1.0f};
  SDL_GPUCommandBuffer *m_CurrentCmdBuff = nullptr;
  SDL_GPURenderPass *m_CurrentRenderPass = nullptr;
  SDL_GPUTexture *m_CurrentTargetTexture = nullptr;
  SDL_GPUSampler *m_DefaultSampler = nullptr;

  bool m_VSync = true;
  bool m_IsDrawing = false;

  // Render queue
  std::vector<RenderCommand> m_RenderCommands;
  std::vector<RenderLayer> m_RenderLayers;

  // resources
  std::vector<VertexBufferInternal> m_VBs;
  std::vector<IndexBufferInternal> m_IBs;
  std::vector<MeshInternal> m_Meshes;
  std::vector<TextureInternal> m_Textures;
  std::vector<ShaderInternal> m_Shaders;
  std::vector<MaterialInternal> m_Materials;
  std::vector<SDL_GPUGraphicsPipeline *> m_Pipelines;
};
} // namespace Mockingbird::SDL::Graphics
