#pragma once

#include "Renderer.hpp"
#include <Core/Application.hpp>
#include <Core/Subsystem.hpp>
#include <Core/Time.hpp>
#include <SDL3/SDL.h>

namespace Pumpkin::SDL::Graphics {
class PESDLGraphiscSubsystem : public Core::PESubsystem {
public:
  PESDLGraphiscSubsystem(SDL_Window *window) : m_Window(window) {}

  void OnBegin() override;
  void OnShutdown() override;
  void OnUpdate(const Core::Time &time) override; // DBG
  void OnRender() override;

private:
  SDL_Window *m_Window = nullptr;
  Renderer m_Renderer;
};
} // namespace Pumpkin::SDL::Graphics
