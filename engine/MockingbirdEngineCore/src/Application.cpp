#include "Core/Application.hpp"

#include "Core/Log.hpp"
#include <chrono>

namespace Mockingbird::Core {
Application *Application::s_App = nullptr;

Application::Application() : m_IsRunning(false) {
  if (s_App) {
    ME_LOG_CORE_ERROR("Application already exists");
    return;
  }
  s_App = this;
}

Application::~Application() { Terminate(); }

void Application::Run() {
  m_IsRunning = true;

  OnStart();

  auto lastTime = std::chrono::high_resolution_clock::now();

  while (m_IsRunning) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    float dt = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;

    if (dt > 0.1f)
      dt = 0.1f;

    m_Time.DeltaTime = dt;
    m_Time.TotalTime += dt;
    m_Time.FrameCount++;

    m_SubsystemManager.UpdateAll(m_Time);

    if (!m_IsRunning)
      break;

    if (m_PreRenderCallback) {
      m_PreRenderCallback();
    }
    m_SubsystemManager.RenderAll();
    if (m_PostRenderCallback) {
      m_PostRenderCallback();
    }
  }

  Terminate();
}

void Application::PostEvent(Event &event) {
  m_SubsystemManager.OnEventAll(event);
}

void Application::Shutdown() { m_IsRunning = false; }

void Application::Terminate() {
  m_SubsystemManager.ShutdownAll();
  s_App = nullptr;
}
} // namespace Mockingbird::Core
