#pragma once

#include "Descriptors.hpp"
#include "GraphicsHandle.hpp"
#include "VertexLayout.hpp"
#include <SDL3/SDL.h>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace Pumpkin::SDL::Graphics {
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

  MeshHandle CreateMesh(std::span<const std::byte> verticies,
                        uint32_t vertexStride,
                        std::span<const std::byte> indicies,
                        uint32_t indexStride);

  ShaderHandle CreateShader(std::span<const std::byte> vert,
                            std::span<const std::byte> frag,
                            const ShaderProgramDesc &desc);

  PipelineHandle CreateGraphicsPipeline(const ShaderHandle &shader,
                                        const VertexLayout &layout);

  void DrawMesh(const MeshHandle &meshHnd, PipelineHandle pipelineHnd);

  inline void SetClearColor(float r, float g, float b, float alpha = 1.0f) {
    m_ClearColor = SDL_FColor{r, g, b, alpha};
  }

  void StartFrame();
  void EndFrame();

  // 2D rendering
  void BeginDraw2D(); // TODO: camera2D and target texture
  void EndDraw2D();

private:
  // struct TextureInternal {
  //   SDL_GPUTexture *Texture;
  //   uint32_t Version;
  // };

  struct MeshInternal {
    SDL_GPUBuffer *VertexBuffer;
    SDL_GPUBuffer *IndexBuffer;
    uint32_t VertexCount;
    uint32_t IndexCount;
    SDL_GPUIndexElementSize IndexFormat;
    uint32_t Version;
  };

  struct ShaderInternal {
    SDL_GPUShader *VertexShader;
    SDL_GPUShader *FragmentShader;
    uint32_t Version;
  };

private:
  SDL_GPUShader *CreateShaderStage(std::span<const std::byte> code,
                                   const ShaderStageDesc &desc,
                                   SDL_GPUShaderStage stage);

private:
  SDL_GPUDevice *m_Device = nullptr;
  SDL_Window *m_Wnd = nullptr;

  SDL_FColor m_ClearColor = SDL_FColor{0.1f, 0.1f, 0.1f, 1.0f};
  SDL_GPUCommandBuffer *m_CurrentCmdBuff = nullptr;
  SDL_GPURenderPass *m_CurrentRenderPass = nullptr;
  SDL_GPUTexture *m_CurrentTargetTexture = nullptr;

  bool m_VSync = true;
  bool m_IsDrawing = false;
  bool m_FirstPassInFrame = false;

  // resources
  std::vector<MeshInternal> m_Meshes;
  std::vector<ShaderInternal> m_Shaders;
  std::vector<SDL_GPUGraphicsPipeline *> m_Pipelines;
};
} // namespace Pumpkin::SDL::Graphics
