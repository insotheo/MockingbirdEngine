#include "Subsystem.hpp"

#include <Core/Application.hpp>
#include <Core/Log.hpp>

namespace Mockingbird::SDL::Graphics {

void MESDLGraphicsSubsystem::OnBegin() {
  if (!SDL_WasInit(SDL_INIT_VIDEO) || !m_Window) {
    ME_LOG_CORE_ERROR("SDL3 Video or Window subsystems wern't initialized "
                      "before adding graphics subsystem");
    return;
  }

  m_Renderer.Init(m_Window);
  if (!m_Renderer.IsCreatedSuccessfully()) {
    ME_LOG_CORE_ERROR("Failed to create Renderer!");
    return;
  }

  Core::Application::GetApp()->SetRenderCallbacks(
      [&]() { m_Renderer.StartFrame(); }, [&]() { m_Renderer.EndFrame(); });
}

void MESDLGraphicsSubsystem::OnShutdown() {
  m_Renderer.Shutdown();
  m_Window = nullptr;
}

} // namespace Mockingbird::SDL::Graphics
