#include <Event/KeyboardEvent.hpp>
#include <Event/MouseEvent.hpp>
#include <MockingbirdEngineCore.hpp>
#include <MockingbirdEngineSDLDearImGui.hpp>
#include <MockingbirdEngineSDLGraphics.hpp>
#include <MockingbirdEngineSDLWindowing.hpp>
#include <imgui.h>

#include <cmath>
#include <cstdint>
#include <vector>

using namespace Mockingbird;

SDL::Windowing::MESDLWindowingSubsystem *wnd;
SDL::Graphics::MESDLGraphicsSubsystem *graphics;

SDL::Graphics::ShaderHandle shader;
SDL::Graphics::MaterialHandle mat;
SDL::Graphics::MeshHandle mesh;
SDL::Graphics::PipelineHandle pipeline;

float bgR = 0.2f, bgG = 1.f, bgB = 0.4f;
Core::MEVec3 rectColor{1.0f, 1.0f, 1.0f};
Core::MEVec3 rectBgColor{0.0f, 0.0f, 0.0f};

class SandboxSubsystem : public Core::MESubsystem {
public:
  SandboxSubsystem() { m_RenderOrder = Core::RenderOrder::Regular; }

  void OnBegin() override {
    ME_LOG_INFO("Game started");

    std::vector<uint8_t> vertCode =
        Core::LoadFileBytes("./assets/hello.vert.spv");
    std::vector<uint8_t> fragCode =
        Core::LoadFileBytes("./assets/hello.frag.spv");

    shader = graphics->GetRenderer().CreateShader(
        vertCode.data(), vertCode.size(), fragCode.data(), fragCode.size());
    pipeline = graphics->GetRenderer().CreateGraphicsPipeline(shader);
    mat = graphics->GetRenderer().CreateMaterial(shader, "MatBuffer", false);

    float verticies[] = {
        // clang-format off
        -0.8f, 0.8f,
        -0.8f, -0.8f,
        0.8f, -0.8f,
        0.8f, 0.8f
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
    Core::MEMat4 matrix(1.0f);

    float angle = time.TotalTime * 0.5f;
    matrix = Core::Math::Rotate(matrix, angle, Core::MEVec3(0.0f, 0.0f, 1.0f));

    float scaleFactor = 1.0f + std::sin(time.TotalTime * 1.2f) * 0.4f;
    matrix =
        Core::Math::Scale(matrix, Core::MEVec3(scaleFactor, scaleFactor, 1.0f));

    matrix[0][3] = std::sin(time.TotalTime * 2.0f) * 0.5f;
    matrix[1][3] = std::cos(time.TotalTime * 1.5f) * 0.5f;

    ImGui::Begin("Menu");

    ImGui::Text("FPS: %.2f", 1.f / time.DeltaTime);

    ImGui::ColorPicker3("Bg", &bgR);
    graphics->GetRenderer().SetClearColor(bgR, bgG, bgB);

    ImGui::ColorPicker3("Rectangle", &rectColor.x);
    ImGui::ColorPicker3("Rectangle bg", &rectBgColor.x);

    graphics->GetRenderer().MaterialSetFloat3(mat, "uColor", rectColor);
    graphics->GetRenderer().MaterialSetFloat3(mat, "uBgColor", rectBgColor);
    graphics->GetRenderer().MaterialSetMat4(mat, "uEffect", matrix);
    graphics->GetRenderer().MaterialSetFloat(
        mat, "uTime", static_cast<float>(time.TotalTime));

    ImGui::End();
  }

  void OnRender() override {
    graphics->GetRenderer().BeginDraw2D();
    graphics->GetRenderer().DrawMesh(mesh, mat, pipeline);
    graphics->GetRenderer().EndDraw2D();
  }

  void OnEvent(Core::Event &event) override {
    Core::EventDispatcher dispatcher(event);

    dispatcher.Dispatch<Core::KeyPressedEvent>(
        [&](Core::KeyPressedEvent &e) { ME_LOG_INFO("{}", e.ToString()); });
    dispatcher.Dispatch<Core::MouseButtonPressedEvent>(
        [&](Core::MouseButtonPressedEvent &e) {
          ME_LOG_INFO("{}", e.ToString());
        });
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
