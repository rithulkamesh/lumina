#include "lumina.hpp"

void Lumina::createSurface() {
  if (glfwCreateWindowSurface(vulkan_instance, window, nullptr,
                              &window_surface) != VK_SUCCESS) {
    Logger::Log(LogLevel_Error, "failed to create window surface!", __FILE__,
                __LINE__);
    exit(1);
  }
}
