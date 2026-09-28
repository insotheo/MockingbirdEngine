#include <PumpkinEngineCore.hpp>
#include <PumpkinEngineSDLGraphics.hpp>

class SandboxApplication : public Pumpkin::Core::Application {
  void OnCreated() override {
    GetSubsystemManager()
        .RegisterSubsystem<Pumpkin::SDL::Graphics::PESDLGraphiscSubsystem>(
            Pumpkin::SDL::Graphics::WindowInfo{
                .Width = 800, .Height = 600, .Title = "Hello, World!"});
  }
};

Pumpkin::Core::Application *CreatePumpkinApplication() {
  return new SandboxApplication();
}
