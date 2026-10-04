#pragma once

#include "Core/SubsystemManager.hpp"
#include "Core/Time.hpp"
#include "Event/Event.hpp"
#include <functional>

namespace Mockingbird::Core {
class Application {
public:
  using RenderCallback = std::function<void()>;

  Application();
  virtual ~Application();

  virtual void OnStart() {}
  void Run();
  void Shutdown();

  void PostEvent(Event &event);

  inline static Application *GetApp() { return s_App; }
  inline SubsystemManager &GetSubsystemManager() { return m_SubsystemManager; }

  inline void SetRenderCallbacks(RenderCallback pre, RenderCallback post) {
    m_PreRenderCallback = std::move(pre);
    m_PostRenderCallback = std::move(post);
  }

private:
  static Application *s_App;
  void Terminate();

  bool m_IsRunning;
  Time m_Time;
  SubsystemManager m_SubsystemManager;

  RenderCallback m_PreRenderCallback = nullptr;
  RenderCallback m_PostRenderCallback = nullptr;
};
} // namespace Mockingbird::Core
