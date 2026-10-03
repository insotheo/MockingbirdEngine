#include "Core/SubsystemManager.hpp"

namespace Pumpkin::Core {

void SubsystemManager::UpdateAll(const Time &time) {
  for (auto &system : m_Subsystems)
    system->OnUpdate(time);
}

void SubsystemManager::RenderAll() {
  for (auto &system : m_Subsystems) {
    if (system.get()->GetRenderOrder() == RenderOrder::Regular)
      system->OnRender();
  }

  for (auto &system : m_Subsystems) { // UI on top
    if (system.get()->GetRenderOrder() == RenderOrder::UI)
      system->OnRender();
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

} // namespace Pumpkin::Core
