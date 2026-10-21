#include "Core/SubsystemManager.hpp"

namespace Mockingbird::Core {

void SubsystemManager::UpdateAll(Scene &scene, const Time &time) {
  for (auto &system : m_Subsystems)
    system->OnUpdate(scene, time);
}

void SubsystemManager::RenderAll(Scene &scene) {
  for (auto &system : m_Subsystems) {
    system->OnRender(scene);
  }
}

void SubsystemManager::OnEventAll(Event &event) {
  for (auto it = m_Subsystems.begin(); it != m_Subsystems.end(); ++it) {
    if (event.Handled)
      break;
    (*it)->OnEvent(event);
  }
}

void SubsystemManager::ShutdownAll() {
  for (auto it = m_Subsystems.rbegin(); it != m_Subsystems.rend(); ++it) {
    (*it)->OnShutdown();
  }
  m_Subsystems.clear();
  m_SubsystemMap.clear();
}

} // namespace Mockingbird::Core
