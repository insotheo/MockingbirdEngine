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
}

void MESDLGraphicsSubsystem::OnRender(Core::Scene &scene) {
  auto &registry = scene.GetRegistry();

  struct CameraTask {
    Core::MEMat4 ViewProj;
    Core::MEVec4 ClearColor;
    SDL_GPUTexture *Target;
    int Order;
  };
  std::vector<CameraTask> camTasks;

  auto camView = registry.view<const Core::MEComponent(Camera)>();
  for (auto entity : camView) {
    const auto &camComp = camView.get<const Core::MEComponent(Camera)>(entity);
    if (!camComp.IsActive)
      continue;

    camTasks.push_back({
        const_cast<Core::Camera &>(camComp.CamInstance)
            .GetViewProjectionMatrix(),
        camComp.ClearColor,
        static_cast<SDL_GPUTexture *>(camComp.TargetTexture),
        camComp.RenderOrder,
    });
  }
  if (camTasks.empty())
    return;

  std::sort(camTasks.begin(), camTasks.end(),
            [](const auto &a, const auto &b) { return a.Order < b.Order; });

  m_Renderer.BeginFrame(camTasks[0].ViewProj);

  for (const auto &task : camTasks) {
    m_Renderer.UpdateViewProjMatrix(task.ViewProj);

    auto renderView = registry.view<const Core::MEComponent(Transform),
                                    const MEComponent(Renderable)>();
    for (auto entity : renderView) {
      const auto &transform =
          renderView.get<const Core::MEComponent(Transform)>(entity);
      const auto &renderable =
          renderView.get<const MEComponent(Renderable)>(entity);

      Core::MEMat4 modelMatrix = Core::Math::Model(
          transform.Position, transform.Scale, transform.Rotation);

      m_Renderer.SubmitMesh(renderable.Mesh, renderable.Pipeline,
                            renderable.Material, modelMatrix);
    }

    m_Renderer.ExecuteRenderPass(task.Target, task.ClearColor);
  }

  m_Renderer.EndFrameAndFlush();
}

void MESDLGraphicsSubsystem::OnShutdown() {
  m_Renderer.Shutdown();
  m_Window = nullptr;
}

} // namespace Mockingbird::SDL::Graphics
