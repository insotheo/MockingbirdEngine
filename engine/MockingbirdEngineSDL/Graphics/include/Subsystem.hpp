#pragma once

#include "Renderer.hpp"
#include <Core/Subsystem.hpp>
#include <Core/Time.hpp>
#include <SDL3/SDL.h>

namespace Mockingbird::SDL::Graphics {
class MESDLGraphicsSubsystem : public Core::MESubsystem {
public:
  MESDLGraphicsSubsystem(SDL_Window *window) : m_Window(window) {}

  void OnBegin() override;
  void OnShutdown() override;

  inline Renderer &GetRenderer() { return m_Renderer; }

private:
  SDL_Window *m_Window = nullptr;
  Renderer m_Renderer;
};
} // namespace Mockingbird::SDL::Graphics
