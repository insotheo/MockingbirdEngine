#pragma once

#include <Core/Subsystem.hpp>
#include <Core/Time.hpp>
#include <Event/Event.hpp>
#include <MockingbirdEngineSDLGraphics.hpp>
#include <SDL3/SDL.h>

namespace Mockingbird::SDL::DearImGui {
class MESDLImGui : public Core::MESubsystem {
public:
  MESDLImGui(SDL_Window *window, Graphics::Renderer *renderer)
      : m_Wnd(window), m_Renderer(renderer), m_Gpu(renderer->GetGPU()) {}

  ~MESDLImGui() {
    m_Wnd = nullptr;
    m_Gpu = nullptr;
  }

  void OnBegin() override;
  void OnShutdown() override;
  void OnEvent(Core::Event &event) override;
  void OnUpdate(Core::Scene &scene, const Core::Time &time) override;

private:
  SDL_Window *m_Wnd;
  SDL_GPUDevice *m_Gpu;
  Graphics::Renderer *m_Renderer;

  void PrepareRender(SDL_GPUCommandBuffer *cmd);
  void RenderImGui(SDL_GPURenderPass *pass);

  bool m_Initialized = false;
  bool m_FrameStarted = false;
};
} // namespace Mockingbird::SDL::DearImGui
