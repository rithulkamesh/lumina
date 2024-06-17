#include "lumina.hpp"
#include "util/log.hpp"
#include <iostream>

int Lumina::Run() {

  Init();

  while (!glfwWindowShouldClose(window)) {
    // glClear(GL_COLOR_BUFFER_BIT); // Deprecated in MacOS 10.14
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  Cleanup();

  return 0;
}

void Lumina::Init() {
  if (!glfwInit())
    return;

  Logger::Log(LogLevel_Info, "glfw initalized successfully.", __FILE__,
              __LINE__);

  window = glfwCreateWindow(window_options.size.x, window_options.size.y,
                            window_options.title.c_str(), NULL, NULL);
  if (!window) {
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(window);

  // Here starts Vulkan stuff
  create_vulkan_instance();
  setupDebugMessenger();
}

void Lumina::Cleanup() {
  Logger::Log(LogLevel_Info, "cleaning up.", __FILE__, __LINE__);

  if (enableValidationLayers)

    DestroyDebugUtilsMessengerEXT(vulkan_instance, debugMessenger, nullptr);

  vkDestroyInstance(vulkan_instance, nullptr);
  glfwDestroyWindow(window);
  glfwTerminate();
}
