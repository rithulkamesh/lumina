#include "lumina.hpp"
#include "util/log.hpp"

void Lumina::create_vulkan_instance() {
  auto version =
      VK_MAKE_VERSION(game_metadata.version[0], game_metadata.version[1],
                      game_metadata.version[2]);
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = game_metadata.identifier.c_str();
  appInfo.applicationVersion = version;
  appInfo.pEngineName = "Lumina";
  appInfo.engineVersion = version;
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // Vulkan requies extensions to interface with the window systems
  uint32_t glfwExtensionCount = 0;
  const char **glfwExtensions;

  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtensions;
  createInfo.enabledLayerCount = 0;

  // MACOS VK_ERROR_INCOMPATIBLE_DRIVER Fixes
  std::vector<const char *> requiredExtensions;

  for (uint32_t i = 0; i < glfwExtensionCount; i++) {
    requiredExtensions.emplace_back(glfwExtensions[i]);
  }

  requiredExtensions.emplace_back(
      VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();

  VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkan_instance);

  if (result != VK_SUCCESS) {
    std::stringstream ss;
    ss << "failed to create instance. code: " << (int)result;
    Logger::Log(LogLevel_Error, ss.str(), __FILE__, __LINE__);
    throw std::runtime_error("failed to create instance!");
  }

  Logger::Log(LogLevel_Info, "vulkan intialized successfully.", __FILE__,
              __LINE__);
}

void Lumina::get_supported_extensions() {
  uint32_t extensionCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
  vector<VkExtensionProperties> extensions(extensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                         extensions.data());
  cout << "available extensions:\n";

  for (const auto &extension : extensions) {
    cout << '\t' << extension.extensionName << '\n';
  }
}