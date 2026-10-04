#pragma once

extern Pumpkin::Core::Application *CreatePumpkinApplication();

int main(int argc, char **agrv) {
  auto app = CreatePumpkinApplication();
  PE_LOG_CORE_TRACE("Application was created successfuly!");
  app->Run();
  delete app;
  PE_LOG_CORE_TRACE("Application was stopped successfuly!");
  return 0;
}
