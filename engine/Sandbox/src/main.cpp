#include <MockingbirdEngineCore.hpp>
#include <MockingbirdEngineSDLDearImGui.hpp>
#include <MockingbirdEngineSDLGraphics.hpp>
#include <MockingbirdEngineSDLWindowing.hpp>
#include <imgui.h>

#include <cstdint>
#include <vector>

using namespace Mockingbird;

SDL::Windowing::MESDLWindowingSubsystem *wnd;
SDL::Graphics::MESDLGraphicsSubsystem *graphics;

SDL::Graphics::ShaderHandle shader;
SDL::Graphics::TextureHandle texture;
SDL::Graphics::MaterialHandle mat;
SDL::Graphics::MeshHandle mesh;
SDL::Graphics::PipelineHandle pipeline;

class SandboxSubsystem : public Core::MESubsystem {
public:
  SandboxSubsystem() { m_RenderOrder = Core::RenderOrder::Regular; }

  void OnBegin() override {
    ME_LOG_INFO("Game started");

    std::vector<uint8_t> vertCode =
        Core::FileSys::LoadFileBytes("./assets/hello.vert.spv");
    std::vector<uint8_t> fragCode =
        Core::FileSys::LoadFileBytes("./assets/hello.frag.spv");
    Core::FileSys::ImageFile img =
        Core::FileSys::LoadImage("./assets/testTexture.png");

    shader = graphics->GetRenderer().CreateShader(
        vertCode.data(), vertCode.size(), fragCode.data(), fragCode.size());
    pipeline = graphics->GetRenderer().CreateGraphicsPipeline(shader);
    mat = graphics->GetRenderer().CreateMaterial(shader, "MatBuffer");
    texture = graphics->GetRenderer().CreateTexture(img);

    Core::FileSys::FreeImage(img);

    float verticies[] = {
        // clang-format off
        -0.8f, 0.8f, 1.0f, 0.0f,
        -0.8f, -0.8f, 0.0f, 0.0f,
        0.8f, -0.8f, 0.0f, 1.0f,
        0.8f, 0.8f, 1.0f, 1.0f,
        // clang-format on
    };
    uint16_t indicies[] = {0, 1, 2, 2, 3, 0};

    auto vb = graphics->GetRenderer().CreateVertexBuffer(
        &verticies, sizeof(verticies) / sizeof(float),
        graphics->GetRenderer().GetVertexLayout(shader)->GetStride());

    auto ib = graphics->GetRenderer().CreateIndexBuffer(
        &indicies, sizeof(indicies) / sizeof(uint16_t), sizeof(uint16_t));

    mesh = graphics->GetRenderer().CreateMesh(vb, ib);
  }

  void OnUpdate(const Core::Time &time) override {
    ImGui::Begin("Menu");
    ImGui::Text("FPS: %.2f", 1.f / time.DeltaTime);
    ImGui::End();

    graphics->GetRenderer().MaterialSetFloat(
        mat, "uTime", static_cast<float>(time.TotalTime));
  }

  void OnRender() override {
    graphics->GetRenderer().BeginDraw2D();
    graphics->GetRenderer().DrawMesh(mesh, mat, texture, pipeline);
    graphics->GetRenderer().EndDraw2D();
  }

  void OnShutdown() override {}
};

class SandboxApplication : public Core::Application {
  void OnStart() override {

    GetSubsystemManager()
        .RegisterSubsystem<SDL::Windowing::MESDLWindowingSubsystem>(
            SDL::Windowing::WindowInfo{
                .Width = 800, .Height = 600, .Title = "Hello, World!"});
    wnd = GetSubsystemManager()
              .GetSubsystem<SDL::Windowing::MESDLWindowingSubsystem>();

    GetSubsystemManager()
        .RegisterSubsystem<SDL::Graphics::MESDLGraphicsSubsystem>(
            wnd->GetSDLWindow());
    graphics = GetSubsystemManager()
                   .GetSubsystem<SDL::Graphics::MESDLGraphicsSubsystem>();

    GetSubsystemManager().RegisterSubsystem<SDL::DearImGui::MESDLImGui>(
        wnd->GetSDLWindow(), &graphics->GetRenderer());

    GetSubsystemManager().RegisterSubsystem<SandboxSubsystem>();
  }
};

Mockingbird::Core::Application *CreateMockingbirdApplication() {
  return new SandboxApplication();
}
