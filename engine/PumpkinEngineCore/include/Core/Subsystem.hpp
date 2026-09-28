#pragma once

#include "Core/Time.hpp"
#include "Event/Event.hpp"

namespace Pumpkin::Core {
class PESubsystem {
public:
  virtual ~PESubsystem() = default;

  virtual void OnBegin() = 0;
  virtual void OnUpdate(const Time &time) {}
  virtual void OnRender() {}
  virtual void OnShutdown() = 0;

  virtual void OnEvent(Event &event) {}
};
} // namespace Pumpkin::Core
