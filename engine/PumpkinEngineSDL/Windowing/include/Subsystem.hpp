#pragma once

#include "WindowInfo.hpp"
#include <Core/Application.hpp>
#include <Core/Subsystem.hpp>
#include <Core/Time.hpp>
#include <SDL3/SDL.h>

namespace Pumpkin::SDL::Windowing {
class PESDLWindowingSubsystem : public Core::PESubsystem {
public:
  PESDLWindowingSubsystem(const WindowInfo &info) : m_WndInfo(info) {}

  void OnBegin() override;
  void OnUpdate(const Core::Time &time) override;
  void OnShutdown() override;

  inline SDL_Window *GetSDLWindow() const { return m_Window; }

private:
  Core::Application *m_App = nullptr;

  WindowInfo m_WndInfo;
  SDL_Window *m_Window = nullptr;
};
} // namespace Pumpkin::SDL::Windowing
