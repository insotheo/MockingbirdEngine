#include "Subsystem.hpp"

#include <Core/Log.hpp>
#include <Event/WindowEvent.hpp>
#include <cstdint>

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

void *PESDLWindowingSubsystem::GetNativeWindowHandle() {
  if (!m_Window)
    return nullptr;

  SDL_PropertiesID props = SDL_GetWindowProperties(m_Window);

#if defined(SDL_PLATFORM_WINDOWS)
  HWND hwnd = (HWND)SDL_GetPointerProperty(
      props, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr);
  return (void *)hwnd;
#elif defined(SDL_PLATFORM_APPLE)
  void *nsWindow = SDL_GetPointerProperty(
      props, SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, nullptr);
  return nsWindow;
#elif defined(SDL_PLATFORM_LINUX)
  if (SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
    return SDL_GetPointerProperty(
        props, SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, nullptr);
  } else { // X11
    uint64_t xwindow =
        SDL_GetNumberProperty(props, SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
    return (void *)xwindow;
  }
#endif
}

} // namespace Pumpkin::SDL::Windowing
