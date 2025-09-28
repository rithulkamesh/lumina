#include "lumina.hpp"

void Lumina::createSurface()
{
  Logger::Log(LogLevel_Info, "creating window surface", __FILE__, __LINE__);
  VkResult result = glfwCreateWindowSurface(vulkan_instance, window, nullptr, &window_surface);
  if (result != VK_SUCCESS)
  {
    Logger::Log(LogLevel_Error, "failed to create window surface! Error code: " + std::to_string(result), __FILE__, __LINE__);
    exit(1);
  }
  Logger::Log(LogLevel_Info, "window surface created successfully", __FILE__, __LINE__);
}
