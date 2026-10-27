#include "ECS/Entity.hpp"
#include <MockingbirdEngineCore.hpp>
#include <MockingbirdEngineSDLDearImGui.hpp>
#include <MockingbirdEngineSDLGraphics.hpp>
#include <MockingbirdEngineSDLWindowing.hpp>
#include <imgui.h>

#include <cstdint>
#include <vector>

using namespace Mockingbird;

float verticies[] = {
    // clang-format off
        -0.8f, 0.8f, 1.0f, 0.0f,
        -0.8f, -0.8f, 0.0f, 0.0f,
        0.8f, -0.8f, 0.0f, 1.0f,
        0.8f, 0.8f, 1.0f, 1.0f,
    // clang-format on
};
uint16_t indicies[] = {0, 1, 2, 2, 3, 0};

Core::EntityHandle spriteID;
Core::EntityHandle cameraID;

Core::VFS *vfs;
SDL::Windowing::MESDLWindowingSubsystem *wnd;
SDL::Graphics::MESDLGraphicsSubsystem *graphics;

class SandboxSubsystem : public Core::MESubsystem {
public:
  void OnBegin() override {
    ME_LOG_INFO("Game started");

    std::vector<uint8_t> vertCode =
        vfs->Open("root/assets/hello.vert.spv")->GetBytes();
    std::vector<uint8_t> fragCode =
        vfs->Open("root/assets/hello.frag.spv")->GetBytes();
    Core::ImageFile img = vfs->Open("root/assets/testTexture.png")->GetImage();

    auto shader = graphics->GetRenderer().CreateShader(
        vertCode.data(), vertCode.size(), fragCode.data(), fragCode.size());

    auto pipeline = graphics->GetRenderer().CreateGraphicsPipeline(shader);

    auto mat = graphics->GetRenderer().CreateMaterial(shader, "MatBuffer");
    auto texture = graphics->GetRenderer().CreateTexture(img);
    graphics->GetRenderer().MaterialSetTexture(mat, 0, texture);

    auto vb = graphics->GetRenderer().CreateVertexBuffer(
        &verticies, sizeof(verticies) / sizeof(float),
        graphics->GetRenderer().GetVertexLayout(shader)->GetStride());

    auto ib = graphics->GetRenderer().CreateIndexBuffer(
        &indicies, sizeof(indicies) / sizeof(uint16_t), sizeof(uint16_t));

    auto mesh = graphics->GetRenderer().CreateMesh(vb, ib);

    Core::Scene testScene;
    Core::Entity sprite = testScene.CreateEntity();
    sprite.AddComponent<SDL::Graphics::MEComponent(Renderable)>(mesh, pipeline,
                                                                mat);
    sprite.AddComponent<Core::MEComponent(Transform)>();

    Core::Entity camera = testScene.CreateEntity();
    camera.AddComponent<Core::MEComponent(Camera)>();

    spriteID = static_cast<Core::EntityHandle>(sprite);
    cameraID = static_cast<Core::EntityHandle>(camera);

    Core::Application::SetCurrentScene(std::move(testScene));
  }

  void OnUpdate(Core::Scene &scene, const Core::Time &time) override {
    m_FpsTimer += time.DeltaTime;
    m_FrameCount++;

    if (m_FpsTimer >= 0.5f) {
      m_LastDisplaydFps = static_cast<float>(m_FrameCount) / m_FpsTimer;
      m_FpsTimer = 0.0f;
      m_FrameCount = 0;
    }

    ImGui::Begin("Menu");
    ImGui::Text("FPS: %.1f", m_LastDisplaydFps);
    ImGui::End();

    Core::Entity sprite(spriteID, &scene); // update sprite
    Core::Entity cam(cameraID, &scene);    // update camera

    graphics->GetRenderer().MaterialSetFloat(
        sprite.GetComponent<SDL::Graphics::MEComponent(Renderable)>().Material,
        "uTime", static_cast<float>(time.TotalTime));

    cam.GetComponent<Core::MEComponent(Camera)>().CamInstance.Rotate(
        Core::MEVec3{
            0.f,
            0.f,
            1.0f,
        });
  }

  void OnShutdown() override {}

private:
  float m_FpsTimer = 0.0f;
  int m_FrameCount = 0;
  float m_LastDisplaydFps = 0.0f;
};

class SandboxApplication : public Core::Application {
  void OnStart() override {
    vfs = GetAppVFS();

    GetSubsystemManager()
        ->RegisterSubsystem<SDL::Windowing::MESDLWindowingSubsystem>(
            SDL::Windowing::WindowInfo{
                .Width = 1280, .Height = 720, .Title = "Hello, World!"});
    wnd = GetSubsystemManager()
              ->GetSubsystem<SDL::Windowing::MESDLWindowingSubsystem>();

    GetSubsystemManager()
        ->RegisterSubsystem<SDL::Graphics::MESDLGraphicsSubsystem>(
            wnd->GetSDLWindow());
    graphics = GetSubsystemManager()
                   ->GetSubsystem<SDL::Graphics::MESDLGraphicsSubsystem>();

    GetSubsystemManager()->RegisterSubsystem<SDL::DearImGui::MESDLImGui>(
        wnd->GetSDLWindow(), &graphics->GetRenderer());

    GetSubsystemManager()->RegisterSubsystem<SandboxSubsystem>();
  }
};

Mockingbird::Core::Application *CreateMockingbirdApplication() {
  return new SandboxApplication();
}
