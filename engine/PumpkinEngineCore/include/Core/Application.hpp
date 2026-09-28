#pragma once

#include "Core/SubsystemManager.hpp"
#include "Core/Time.hpp"
#include "Event/Event.hpp"

namespace Pumpkin::Core {
class Application {
public:
  Application();
  virtual ~Application();

  virtual void OnCreated() {}
  void Run();
  void Shutdown();

  void PostEvent(Event &event);

  inline static Application *GetApp() { return s_App; }
  inline SubsystemManager &GetSubsystemManager() { return m_SubsystemManager; }

private:
  static Application *s_App;
  void Terminate();

  bool m_IsRunning;
  Time m_Time;
  SubsystemManager m_SubsystemManager;
};
} // namespace Pumpkin::Core
