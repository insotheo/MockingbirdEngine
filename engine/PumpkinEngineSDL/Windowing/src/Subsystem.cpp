#include "Subsystem.hpp"

#include <Core/Log.hpp>
#include <Event/WindowEvent.hpp>

namespace Pumpkin::SDL::Windowing {

void PESDLWindowingSubsystem::OnBegin() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    PE_LOG_CORE_ERROR("Failed to initialize SDL3 Video: {}", SDL_GetError());
    return;
  }

  m_Window = SDL_CreateWindow(m_WndInfo.Title.c_str(), m_WndInfo.Width,
                              m_WndInfo.Height, SDL_WINDOW_RESIZABLE);

  if (!m_Window) {
    PE_LOG_CORE_ERROR("Failed to initialize SDL3 Window: ", SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return;
  }

  m_App = Core::Application::GetApp();
}

void PESDLWindowingSubsystem::OnUpdate(const Core::Time &time) {
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

void PESDLWindowingSubsystem::OnShutdown() {
  if (!m_Window)
    return;

  SDL_DestroyWindow(m_Window);
  SDL_QuitSubSystem(SDL_INIT_VIDEO);

  m_Window = nullptr;
  m_App = nullptr;
}

} // namespace Pumpkin::SDL::Windowing
