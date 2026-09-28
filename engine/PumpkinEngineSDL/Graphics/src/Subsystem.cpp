#include "Subsystem.hpp"

#include <Core/Log.hpp>
#include <cmath>

Pumpkin::Core::Time t;

namespace Pumpkin::SDL::Graphics {

void PESDLGraphiscSubsystem::OnBegin() {
  if (!SDL_WasInit(SDL_INIT_VIDEO) || !m_Window) {
    PE_LOG_CORE_ERROR("SDL3 Video or Window subsystems wern't initialized "
                      "before adding graphics subsystem");
    return;
  }

  m_Renderer.Init(m_Window);
  if (!m_Renderer.IsCreatedSuccessfully()) {
    PE_LOG_CORE_ERROR("Failed to create Renderer!");
    return;
  }
}

void PESDLGraphiscSubsystem::OnUpdate(const Core::Time &time) {
  t = time;
} // DBG

void PESDLGraphiscSubsystem::OnShutdown() {
  m_Renderer.Shutdown();
  m_Window = nullptr;
}

void PESDLGraphiscSubsystem::OnRender() {
  float r = std::sin(t.TotalTime * 1.5f + 0.0f) * 0.5f + 0.5f;
  float g = std::sin(t.TotalTime * 1.5f + 2.0f) * 0.5f + 0.5f;
  float b = std::sin(t.TotalTime * 1.5f + 4.0f) * 0.5f + 0.5f;

  m_Renderer.BeginDraw2D();
  m_Renderer.Clear(r, g, b);
  m_Renderer.EndDraw2D();
}

} // namespace Pumpkin::SDL::Graphics
