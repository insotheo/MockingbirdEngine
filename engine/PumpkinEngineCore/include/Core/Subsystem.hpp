#pragma once

#include "Core/Time.hpp"
#include "Event/Event.hpp"

namespace Pumpkin::Core {

enum class RenderOrder { None, Regular, UI };

class PESubsystem {
public:
  virtual ~PESubsystem() = default;

  virtual void OnBegin() = 0;
  virtual void OnUpdate(const Time &time) {}
  virtual void OnRender() {}
  virtual void OnShutdown() = 0;

  virtual void OnEvent(Event &event) {}

  inline RenderOrder GetRenderOrder() { return m_RenderOrder; }

protected:
  RenderOrder m_RenderOrder = RenderOrder::None;
};
} // namespace Pumpkin::Core
