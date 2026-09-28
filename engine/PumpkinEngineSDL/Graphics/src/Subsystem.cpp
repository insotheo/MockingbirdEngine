#include "Subsystem.hpp"

#include <Core/Log.hpp>
#include <Event/WindowEvent.hpp>
#include <cmath>

Pumpkin::Core::Time t;

namespace Pumpkin::SDL::Graphics {

void PESDLGraphiscSubsystem::OnBegin() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    PE_LOG_CORE_ERROR("Failed to initialize SDL3 Video: ", SDL_GetError());
    return;
  }

  m_Window = SDL_CreateWindow(m_WndInfo.Title.c_str(), m_WndInfo.Width,
                              m_WndInfo.Height, SDL_WINDOW_RESIZABLE);
  if (!m_Window) {
    PE_LOG_CORE_ERROR("Failed to initialize SDL3 Window: ", SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return;
  }

  m_Renderer.Init(m_Window);
  if (!m_Renderer.IsCreatedSuccessfully()) {
    PE_LOG_CORE_ERROR("Failed to create Renderer!");
    SDL_DestroyWindow(m_Window);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return;
  }

  m_App = Core::Application::GetApp();
}

void PESDLGraphiscSubsystem::OnUpdate(const Core::Time &time) {
  t = time;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      Core::WindowCloseEvent close;
      m_App->PostEvent(close);
      if (!close.Handled)
        m_App->Shutdown();
    }
  }
}

void PESDLGraphiscSubsystem::OnShutdown() {
  m_Renderer.Shutdown();

  if (!m_Window)
    return;

  SDL_DestroyWindow(m_Window);

  m_Window = nullptr;
  m_App = nullptr;

  SDL_QuitSubSystem(SDL_INIT_VIDEO);
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
