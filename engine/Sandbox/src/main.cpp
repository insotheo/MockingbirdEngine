#include <Event/KeyboardEvent.hpp>
#include <Event/MouseEvent.hpp>
#include <PumkinEngineSDLDearImGui.hpp>
#include <PumpkinEngineCore.hpp>
#include <PumpkinEngineSDLGraphics.hpp>
#include <PumpkinEngineSDLWindowing.hpp>
#include <imgui.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

using namespace Pumpkin;

SDL::Windowing::PESDLWindowingSubsystem *wnd;
SDL::Graphics::PESDLGraphicsSubsystem *graphics;

SDL::Graphics::BufferLayout layout({SDL::Graphics::PEBufferAttribute(
    SDL::Graphics::ShaderDataType::Float2, "vPos")});

SDL::Graphics::BufferLayout matLayout(
    {SDL::Graphics::PEBufferAttribute(SDL::Graphics::ShaderDataType::Float3,
                                      "uColor"),
     SDL::Graphics::PEBufferAttribute(SDL::Graphics::ShaderDataType::Float,
                                      "uTime")},
    SDL::Graphics::LayoutType::UniformStd140);

SDL::Graphics::ShaderHandle shader;
SDL::Graphics::MaterialHandle mat;
SDL::Graphics::MeshHandle mesh;
SDL::Graphics::PipelineHandle pipeline;

float bgR = 0.2f, bgG = 1.f, bgB = 0.4f;
Core::PEVec3 trColor{1.0f, 1.0f, 1.0f};

class SandboxSubsystem : public Core::PESubsystem {
public:
  SandboxSubsystem() { m_RenderOrder = Core::RenderOrder::Regular; }

  void OnBegin() override {
    PE_LOG_INFO("Game started");

    std::vector<std::byte> vertCode =
        Core::LoadFileBytes("./triangle.vert.spv");
    std::vector<std::byte> fragCode =
        Core::LoadFileBytes("./triangle.frag.spv");

    shader = graphics->GetRenderer().CreateShader(
        vertCode, fragCode, {.Fragment = {.UniformBufferCount = 1}});
    pipeline = graphics->GetRenderer().CreateGraphicsPipeline(shader, layout);
    mat = graphics->GetRenderer().CreateMaterial(pipeline, matLayout);

    std::vector<float> verticies = {
        // clang-format off
        0.0f, 0.5f,
        -0.5f, -0.5f,
        0.5f, -0.5f
        // clang-format on
    };
    std::vector<uint16_t> indicies = {0, 1, 2};

    mesh = graphics->GetRenderer().CreateMesh(
        std::as_bytes(std::span(verticies)), layout.GetStride(),
        std::as_bytes(std::span(indicies)), sizeof(uint16_t));
  }

  void OnUpdate(const Core::Time &time) override {
    ImGui::Begin("Menu");

    ImGui::Text("FPS: %.2f", 1.f / time.DeltaTime);

    ImGui::ColorPicker3("Bg", &bgR);
    graphics->GetRenderer().SetClearColor(bgR, bgG, bgB);

    ImGui::ColorPicker3("Triangle", &trColor.x);
    graphics->GetRenderer().MaterialSetFloat3(mat, "uColor", trColor);
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
        [&](Core::KeyPressedEvent &e) { PE_LOG_INFO("{}", e.ToString()); });
    dispatcher.Dispatch<Core::MouseButtonPressedEvent>(
        [&](Core::MouseButtonPressedEvent &e) {
          PE_LOG_INFO("{}", e.ToString());
        });
  }

  void OnShutdown() override {}
};

class SandboxApplication : public Core::Application {
  void OnStart() override {

    GetSubsystemManager()
        .RegisterSubsystem<SDL::Windowing::PESDLWindowingSubsystem>(
            SDL::Windowing::WindowInfo{
                .Width = 800, .Height = 600, .Title = "Hello, World!"});
    wnd = GetSubsystemManager()
              .GetSubsystem<SDL::Windowing::PESDLWindowingSubsystem>();

    GetSubsystemManager()
        .RegisterSubsystem<SDL::Graphics::PESDLGraphicsSubsystem>(
            wnd->GetSDLWindow());
    graphics = GetSubsystemManager()
                   .GetSubsystem<SDL::Graphics::PESDLGraphicsSubsystem>();

    GetSubsystemManager().RegisterSubsystem<SDL::DearImGui::PESDLImGui>(
        wnd->GetSDLWindow(), &graphics->GetRenderer());

    GetSubsystemManager().RegisterSubsystem<SandboxSubsystem>();
  }
};

Pumpkin::Core::Application *CreatePumpkinApplication() {
  return new SandboxApplication();
}
