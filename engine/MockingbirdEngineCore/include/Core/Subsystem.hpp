#pragma once

#include "Core/Time.hpp"
#include "ECS/Scene.hpp"
#include "Event/Event.hpp"

namespace Mockingbird::Core {

enum class RenderOrder { None, Regular, UI };

class MESubsystem {
public:
  virtual ~MESubsystem() = default;

  virtual void OnBegin() = 0;
  virtual void OnUpdate(Scene &scene, const Time &time) {}
  virtual void OnRender(Scene &scene) {}
  virtual void OnShutdown() = 0;

  virtual void OnEvent(Event &event) {}

  inline RenderOrder GetRenderOrder() { return m_RenderOrder; }

protected:
  RenderOrder m_RenderOrder = RenderOrder::None;
};
} // namespace Mockingbird::Core
