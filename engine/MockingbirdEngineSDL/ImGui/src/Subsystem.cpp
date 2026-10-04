#include "MockingbirdEngineSDLDearImGui.hpp"

#include <Core/Log.hpp>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

namespace Mockingbird::SDL::DearImGui {

void MESDLImGui::OnBegin() {
  if (!m_Wnd || !m_Gpu || !m_Renderer) {
    ME_LOG_CORE_ERROR("Failed to initilize DearImGui: window or gpu device "
                      "pointer in nullptr.");
    return;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplSDLGPU3_InitInfo initInfo{};
  initInfo.Device = m_Gpu;
  initInfo.ColorTargetFormat = SDL_GetGPUSwapchainTextureFormat(m_Gpu, m_Wnd);

  ImGui_ImplSDL3_InitForSDLGPU(m_Wnd);
  ImGui_ImplSDLGPU3_Init(&initInfo);

  m_Initialized = true;
}

void MESDLImGui::OnUpdate(const Core::Time &time) {
  if (!m_Initialized)
    return;

  ImGui_ImplSDLGPU3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();
}

void MESDLImGui::OnRender() {
  if (!m_Initialized)
    return;

  SDL_GPUCommandBuffer *cmdBuff = m_Renderer->GetGPUCurrentCommandBuffer();
  if (!cmdBuff) {
    ImGui::EndFrame();
    return;
  }

  ImGui::Render();
  ImGui_ImplSDLGPU3_PrepareDrawData(ImGui::GetDrawData(), cmdBuff);

  m_Renderer->BeginDraw2D();
  ImGui_ImplSDLGPU3_RenderDrawData(ImGui::GetDrawData(), cmdBuff,
                                   m_Renderer->GetGPUCurrentRenderPass());
  m_Renderer->EndDraw2D();
}

void MESDLImGui::OnEvent(Core::Event &event) {
  if (!m_Initialized || !event.NativeEvent)
    return;

  const SDL_Event *sdlEvent = static_cast<const SDL_Event *>(event.NativeEvent);

  ImGui_ImplSDL3_ProcessEvent(sdlEvent);

  ImGuiIO &io = ImGui::GetIO();

  if ((sdlEvent->type == SDL_EVENT_KEY_DOWN ||
       sdlEvent->type == SDL_EVENT_KEY_UP) &&
      io.WantCaptureKeyboard) {
    event.Handled = true;
  }
  if (sdlEvent->type == SDL_EVENT_TEXT_INPUT && io.WantTextInput) {
    event.Handled = true;
  }
  if ((sdlEvent->type >= SDL_EVENT_MOUSE_MOTION &&
       sdlEvent->type <= SDL_EVENT_MOUSE_WHEEL) &&
      io.WantCaptureMouse) {
    event.Handled = true;
  }
}

void MESDLImGui::OnShutdown() {
  if (!m_Initialized)
    return;

  SDL_WaitForGPUIdle(m_Gpu);
  ImGui_ImplSDLGPU3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();
}

} // namespace Mockingbird::SDL::DearImGui
