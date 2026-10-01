#include "Subsystem.hpp"

#include <Core/Log.hpp>

namespace Pumpkin::SDL::Graphics {

void PESDLGraphicsSubsystem::OnBegin() {
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

void PESDLGraphicsSubsystem::OnShutdown() {
  m_Renderer.Shutdown();
  m_Window = nullptr;
}

} // namespace Pumpkin::SDL::Graphics
