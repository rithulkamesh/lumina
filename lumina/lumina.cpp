#include "lumina.hpp"
#include "util/log.hpp"
#include "vulkan/vulkan_core.h"
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

  glfwWindowHint(GLFW_CLIENT_API,
                 GLFW_NO_API); // Stop creating OpenGL windows since we're doing
                               // it through vulkan refer `engine/window.cpp`
  window = glfwCreateWindow(window_options.size.x, window_options.size.y,
                            window_options.title.c_str(), NULL, NULL);
  if (!window) {
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(window);

  create_vulkan_instance();
}

void Lumina::Cleanup() {
  Logger::Log(LogLevel_Info, "cleaning up.", __FILE__, __LINE__);

  if (enableValidationLayers)
    DestroyDebugUtilsMessengerEXT(vulkan_instance, debugMessenger, nullptr);

  vkDestroyDevice(device, nullptr);
  vkDestroySurfaceKHR(vulkan_instance, window_surface, nullptr);
  vkDestroyInstance(vulkan_instance, nullptr);
  glfwDestroyWindow(window);
  glfwTerminate();
}
