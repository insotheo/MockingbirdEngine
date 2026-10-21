#pragma once

#include "Core/Time.hpp"
#include "ECS/Scene.hpp"
#include "Event/Event.hpp"

namespace Mockingbird::Core {

class MESubsystem {
public:
  virtual ~MESubsystem() = default;

  virtual void OnBegin() = 0;
  virtual void OnUpdate(Scene &scene, const Time &time) {}
  virtual void OnRender(Scene &scene) {}
  virtual void OnShutdown() = 0;

  virtual void OnEvent(Event &event) {}
};
} // namespace Mockingbird::Core
