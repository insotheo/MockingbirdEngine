#pragma once

#include "WindowInfo.hpp"
#include <Core/Application.hpp>
#include <Core/Subsystem.hpp>
#include <Core/Time.hpp>
#include <SDL3/SDL.h>

namespace Mockingbird::SDL::Windowing {
class MESDLWindowingSubsystem : public Core::MESubsystem {
public:
  MESDLWindowingSubsystem(const WindowInfo &info) : m_WndInfo(info) {}

  void OnBegin() override;
  void OnUpdate(const Core::Time &time) override;
  void OnShutdown() override;

  inline SDL_Window *GetSDLWindow() const { return m_Window; }
  void *GetNativeWindowHandle();

private:
  Core::Application *m_App = nullptr;

  WindowInfo m_WndInfo;
  SDL_Window *m_Window = nullptr;
};
} // namespace Mockingbird::SDL::Windowing
