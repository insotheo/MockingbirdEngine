#include "Subsystem.hpp"

#include "GraphicsHandle.hpp"
#include "VertexLayout.hpp"
#include <Core/FileSys.hpp>
#include <Core/Log.hpp>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

Pumpkin::Core::Time t;
std::vector<std::byte> vertCode =
    Pumpkin::Core::LoadFileBytes("./triangle.vert.spv");
std::vector<std::byte> fragCode =
    Pumpkin::Core::LoadFileBytes("./triangle.frag.spv");
Pumpkin::SDL::Graphics::ShaderHandle shader;
Pumpkin::SDL::Graphics::MeshHandle mesh;
Pumpkin::SDL::Graphics::PipelineHandle pipeline;

namespace Pumpkin::SDL::Graphics {

void PESDLGraphiscSubsystem::OnBegin() {
  if (!SDL_WasInit(SDL_INIT_VIDEO) || !m_Window) {
    PE_LOG_CORE_ERROR("SDL3 Video or Window subsystems wern't initialized "
                      "before adding graphics subsystem");
    return;
  }

  m_Renderer.Init(m_Window);
  if (!m_Renderer.IsCreatedSuccessfully()) {
    PE_LOG_CORE_ERROR("Failed to create Renderer!");
    return;
  }

  // DBG
  shader = m_Renderer.CreateShader(vertCode, fragCode, {});
  pipeline = m_Renderer.CreateGraphicsPipeline(
      shader,
      {.VertexSize = 2 * sizeof(float),
       .Attributes = {PEVertexAttribute{
           .Location = 0, .Format = PEVertexFormat::Float2, .Offset = 0}}});

  std::vector<float> verticies = {
      // clang-format off
    0.0f, 0.5f,
    -0.5f, -0.5f,
    0.5f, -0.5f,
      // clang-format on
  };
  std::vector<uint16_t> indicies = {0, 1, 2};

  mesh = m_Renderer.CreateMesh(
      std::as_bytes(std::span(verticies)), 2 * sizeof(float),
      std::as_bytes(std::span(indicies)), sizeof(uint16_t));
}

void PESDLGraphiscSubsystem::OnUpdate(const Core::Time &time) {
  t = time;
} // DBG

void PESDLGraphiscSubsystem::OnShutdown() {
  m_Renderer.Shutdown();
  m_Window = nullptr;
}

void PESDLGraphiscSubsystem::OnRender() {
  float r = std::sin(t.TotalTime * 1.5f + 0.0f) * 0.5f + 0.5f;
  float g = std::sin(t.TotalTime * 1.5f + 2.0f) * 0.5f + 0.5f;
  float b = std::sin(t.TotalTime * 1.5f + 4.0f) * 0.5f + 0.5f;

  m_Renderer.BeginDraw2D();
  m_Renderer.Clear(r, g, b);
  m_Renderer.DrawMesh(mesh, pipeline);
  m_Renderer.EndDraw2D();
}

} // namespace Pumpkin::SDL::Graphics
