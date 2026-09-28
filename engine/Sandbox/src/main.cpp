#include <PumpkinEngineCore.hpp>
#include <PumpkinEngineSDLGraphics.hpp>
#include <PumpkinEngineSDLWindowing.hpp>

class SandboxApplication : public Pumpkin::Core::Application {
  void OnCreated() override {

    GetSubsystemManager()
        .RegisterSubsystem<Pumpkin::SDL::Windowing::PESDLWindowingSubsystem>(
            Pumpkin::SDL::Windowing::WindowInfo{
                .Width = 800, .Height = 600, .Title = "Hello, World!"});

    GetSubsystemManager()
        .RegisterSubsystem<Pumpkin::SDL::Graphics::PESDLGraphiscSubsystem>(
            GetSubsystemManager()
                .GetSubsystem<
                    Pumpkin::SDL::Windowing::PESDLWindowingSubsystem>()
                ->GetSDLWindow());

    PE_LOG_INFO(
        "{}",
        GetSubsystemManager()
            .GetSubsystem<Pumpkin::SDL::Windowing::PESDLWindowingSubsystem>()
            ->GetNativeWindowHandle());
  }
};

Pumpkin::Core::Application *CreatePumpkinApplication() {
  return new SandboxApplication();
}
