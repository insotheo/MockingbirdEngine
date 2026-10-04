#pragma once

extern Mockingbird::Core::Application *CreateMockingbirdApplication();

int main(int argc, char **agrv) {
  auto app = CreateMockingbirdApplication();
  ME_LOG_CORE_TRACE("Application was created successfuly!");
  app->Run();
  delete app;
  ME_LOG_CORE_TRACE("Application was stopped successfuly!");
  return 0;
}
