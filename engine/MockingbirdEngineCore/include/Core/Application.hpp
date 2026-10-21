#pragma once

#include "Core/Debug.hpp"
#include "Core/SubsystemManager.hpp"
#include "Core/Time.hpp"
#include "ECS/Scene.hpp"
#include "ECS/SceneHandle.hpp"
#include "Event/Event.hpp"
#include "VFS/VFS.hpp"

namespace Mockingbird::Core {
class Application {
public:
  Application();
  virtual ~Application();

  virtual void OnStart() {}
  void Run();
  void Shutdown();

  void PostEvent(Event &event);

  inline static Application *GetApp() { return s_App; }
  inline SubsystemManager *GetSubsystemManager() { return &m_SubsystemManager; }
  inline VFS *GetAppVFS() { return &m_VFS; }

  static inline SceneHandle GetCurrentSceneHandle() {
    ME_ASSERT(s_App != nullptr,
              "Cannot get current scene. Application was not created.");
    return SceneHandle{&(s_App->m_CurrentScene)};
  }
  static inline void SetCurrentScene(Scene &&scene) {
    ME_ASSERT(s_App != nullptr,
              "Cannot set current scene. Application was not created.");
    s_App->m_CurrentScene = std::move(scene);
  }

private:
  static Application *s_App;
  void Terminate();

  bool m_IsRunning;
  Time m_Time;
  SubsystemManager m_SubsystemManager;
  VFS m_VFS;
  Scene m_CurrentScene;
};
} // namespace Mockingbird::Core
