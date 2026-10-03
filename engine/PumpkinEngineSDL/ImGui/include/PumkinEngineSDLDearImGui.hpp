#pragma once

#include <Core/Subsystem.hpp>
#include <Core/Time.hpp>
#include <Event/Event.hpp>
#include <PumpkinEngineSDLGraphics.hpp>
#include <SDL3/SDL.h>

namespace Pumpkin::SDL::DearImGui {
class PESDLImGui : public Core::PESubsystem {
public:
  PESDLImGui(SDL_Window *window, Graphics::Renderer *renderer)
      : m_Wnd(window), m_Renderer(renderer), m_Gpu(renderer->GetGPU()) {}
  ~PESDLImGui() {
    m_Wnd = nullptr;
    m_Gpu = nullptr;
  }

  void OnBegin() override;
  void OnShutdown() override;
  //   void OnEvent(Core::Event &event) override;
  void OnUpdate(const Core::Time &time) override;
  void OnRender() override;

private:
  SDL_Window *m_Wnd;
  SDL_GPUDevice *m_Gpu;
  Graphics::Renderer *m_Renderer;

  bool m_Initialized = false;
};
} // namespace Pumpkin::SDL::DearImGui
