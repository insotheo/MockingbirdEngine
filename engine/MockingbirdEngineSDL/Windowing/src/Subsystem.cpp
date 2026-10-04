#include "Subsystem.hpp"

#include <Core/Log.hpp>
#include <Event/KeyboardEvent.hpp>
#include <Event/MouseEvent.hpp>
#include <Event/WindowEvent.hpp>
#include <cstdint>

namespace Mockingbird::SDL::Windowing {

void MESDLWindowingSubsystem::OnBegin() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    ME_LOG_CORE_ERROR("Failed to initialize SDL3 Video: {}", SDL_GetError());
    return;
  }

  m_Window = SDL_CreateWindow(m_WndInfo.Title.c_str(), m_WndInfo.Width,
                              m_WndInfo.Height, SDL_WINDOW_RESIZABLE);

  if (!m_Window) {
    ME_LOG_CORE_ERROR("Failed to initialize SDL3 Window: ", SDL_GetError());
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    return;
  }

  m_App = Core::Application::GetApp();
}

void MESDLWindowingSubsystem::OnUpdate(const Core::Time &time) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      Core::WindowCloseEvent close;
      close.NativeEvent = &event;
      m_App->PostEvent(close);
      if (!close.Handled)
        m_App->Shutdown();
    } else if (event.type == SDL_EVENT_WINDOW_RESIZED) {
      Core::WindowResizeEvent resize(static_cast<uint32_t>(event.window.data1),
                                     static_cast<uint32_t>(event.window.data2));
      resize.NativeEvent = &event;
      m_App->PostEvent(resize);
    } else if (event.type == SDL_EVENT_WINDOW_MOVED) {
      Core::WindowMoveEvent move(static_cast<uint32_t>(event.window.data1),
                                 static_cast<uint32_t>(event.window.data2));
      move.NativeEvent = &event;
      m_App->PostEvent(move);
    } else if (event.type == SDL_EVENT_KEY_DOWN) {
      Core::KeyPressedEvent pressed(event.key.scancode);
      pressed.NativeEvent = &event;
      m_App->PostEvent(pressed);
    } else if (event.type == SDL_EVENT_TEXT_INPUT) {
      Core::KeyTypedEvent typed(*event.text.text);
      typed.NativeEvent = &event;
      m_App->PostEvent(typed);
    } else if (event.type == SDL_EVENT_KEY_UP) {
      Core::KeyReleasedEvent released(event.key.scancode);
      released.NativeEvent = &event;
      m_App->PostEvent(released);
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      Core::MouseButtonPressedEvent pressed(event.button.button);
      pressed.NativeEvent = &event;
      m_App->PostEvent(pressed);
    } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
      Core::MouseButtonReleasedEvent released(event.button.button);
      released.NativeEvent = &event;
      m_App->PostEvent(released);
    } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
      Core::MouseMovedEvent move(event.motion.x, event.motion.y);
      move.NativeEvent = &event;
      m_App->PostEvent(move);
    } else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
      Core::MouseScrollEvent scroll(event.wheel.x, event.wheel.y);
      scroll.NativeEvent = &event;
      m_App->PostEvent(scroll);
    }
  }
}

void MESDLWindowingSubsystem::OnShutdown() {
  if (!m_Window)
    return;

  SDL_DestroyWindow(m_Window);
  SDL_QuitSubSystem(SDL_INIT_VIDEO);

  m_Window = nullptr;
  m_App = nullptr;
}

void *MESDLWindowingSubsystem::GetNativeWindowHandle() {
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

} // namespace Mockingbird::SDL::Windowing
