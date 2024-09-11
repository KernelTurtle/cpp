// Copyright 2024 Keys
#include "file_handler/file_handler.hpp"
#include "splash_screen/splash_screen.hpp"
#include "tui/tui.hpp"

int main() {
  RenderSplashScreen();

  std::string cpp_folder = GetCurrentWorkingDir();
  TuiSelectAndRun(cpp_folder);
  return 0;
}
