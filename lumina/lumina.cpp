#include "lumina.hpp"
#include <iostream>

int Lumina::Run() {
  Init();

  while (!glfwWindowShouldClose(window)) {
    // glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  Cleanup();

  return 0;
}

void Lumina::Init() {
  if (!glfwInit())
    return;

  window = glfwCreateWindow(window_options.size.x, window_options.size.y,
                            window_options.title.c_str(), NULL, NULL);
  if (!window) {
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(window);
}

void Lumina::Cleanup() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
