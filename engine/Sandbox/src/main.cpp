#include <PumkinEngineSDLDearImGui.hpp>
#include <PumpkinEngineCore.hpp>
#include <PumpkinEngineSDLGraphics.hpp>
#include <PumpkinEngineSDLWindowing.hpp>

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

using namespace Pumpkin;

SDL::Windowing::PESDLWindowingSubsystem *wnd;
SDL::Graphics::PESDLGraphicsSubsystem *graphics;

SDL::Graphics::VertexLayout
    layout({Pumpkin::SDL::Graphics::PEVertexAttribute(
                Pumpkin::SDL::Graphics::ShaderDataType::Float2, "inPos"),
            Pumpkin::SDL::Graphics::PEVertexAttribute(
                Pumpkin::SDL::Graphics::ShaderDataType::Float3, "inColor")});

SDL::Graphics::ShaderHandle shader;
SDL::Graphics::MeshHandle mesh;
SDL::Graphics::PipelineHandle pipeline;

class SandboxSubsystem : public Core::PESubsystem {
public:
  void OnBegin() override {
    PE_LOG_INFO("Game started");

    std::vector<std::byte> vertCode =
        Core::LoadFileBytes("./triangle.vert.spv");
    std::vector<std::byte> fragCode =
        Core::LoadFileBytes("./triangle.frag.spv");

    shader = graphics->GetRenderer().CreateShader(vertCode, fragCode, {});
    pipeline = graphics->GetRenderer().CreateGraphicsPipeline(shader, layout);

    std::vector<float> verticies = {
        // clang-format off
        -0.75f, 0.75f,     1.0f, 0.0f, 0.0f,
        -0.75f, -0.75f,   0.0f, 1.0f, 0.0f,
        0.75f, -0.75f, 0.0f, 0.0f, 1.0f,
        0.75f, 0.75f, 0.3f, 0.5f, 0.5f,
        // clang-format on
    };
    std::vector<uint16_t> indicies = {0, 1, 2, 2, 3, 0};

    mesh = graphics->GetRenderer().CreateMesh(
        std::as_bytes(std::span(verticies)), layout.GetStride(),
        std::as_bytes(std::span(indicies)), sizeof(uint16_t));

    graphics->GetRenderer().SetClearColor(0.2f, 1.f, 0.4f);
  }
  void OnRender() override {
    graphics->GetRenderer().BeginDraw2D();
    graphics->GetRenderer().DrawMesh(mesh, pipeline);
    graphics->GetRenderer().EndDraw2D();
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
