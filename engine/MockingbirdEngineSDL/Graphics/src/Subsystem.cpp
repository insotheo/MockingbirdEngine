#include "Graphics/Subsystem.hpp"

#include "Graphics/Component.hpp"
#include <Core/Application.hpp>
#include <Core/Log.hpp>

namespace Mockingbird::SDL::Graphics {

void MESDLGraphicsSubsystem::OnBegin() {
  if (!SDL_WasInit(SDL_INIT_VIDEO) || !m_Window) {
    ME_LOG_CORE_ERROR("SDL3 Video or Window subsystems wern't initialized "
                      "before adding graphics subsystem");
    return;
  }

  m_Renderer.Init(m_Window);
  if (!m_Renderer.IsCreatedSuccessfully()) {
    ME_LOG_CORE_ERROR("Failed to create Renderer!");
    return;
  }

  Core::Application::GetApp()->SetRenderCallbacks(
      [&]() { m_Renderer.StartFrame(); }, [&]() { m_Renderer.EndFrame(); });
}

void MESDLGraphicsSubsystem::OnRender(Core::Scene &scene) {
  auto &registry = scene.GetRegistry();

  m_Renderer.BeginDraw2D();
  auto view = registry.view<const MEComponent(Renderable2D)>();

  for (auto entity : view) {
    const auto &renderable2D =
        view.get<const MEComponent(Renderable2D)>(entity);

    m_Renderer.DrawMesh(renderable2D.Mesh, renderable2D.Material,
                        renderable2D.Texture, renderable2D.Pipeline);
  }

  m_Renderer.EndDraw2D();
}

void MESDLGraphicsSubsystem::OnShutdown() {
  m_Renderer.Shutdown();
  m_Window = nullptr;
}

} // namespace Mockingbird::SDL::Graphics
