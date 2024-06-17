#include "lumina.hpp"
#include "util/log.hpp"

void Lumina::create_vulkan_instance() {

  if (enableValidationLayers && !checkValidationLayerSupport()) {
    Logger::Log(LogLevel_Error,
                "validation layers requested, but not available!", __FILE__,
                __LINE__);

    throw std::runtime_error("validation layers requested, but not available!");
    return;
  }

  VkApplicationInfo appInfo{};
  VkInstanceCreateInfo createInfo{};

  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    Logger::Log(LogLevel_Info, "validation layers initialized.", __FILE__,
                __LINE__);
  } else {
    createInfo.enabledLayerCount = 0;
  }

  auto version =
      VK_MAKE_VERSION(game_metadata.version[0], game_metadata.version[1],
                      game_metadata.version[2]);
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = game_metadata.identifier.c_str();
  appInfo.applicationVersion = version;
  appInfo.pEngineName = "Lumina";
  appInfo.engineVersion = version;
  appInfo.apiVersion = VK_API_VERSION_1_0;

  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // Vulkan requies extensions to interface with the window systems
  auto extensions = getRequiredExtensions();
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  // MACOS VK_ERROR_INCOMPATIBLE_DRIVER Fixes
  std::vector<const char *> requiredExtensions;

  for (uint32_t i = 0; i < extensions.size(); i++) {
    requiredExtensions.emplace_back(extensions[i]);
  }

  requiredExtensions.emplace_back(
      VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

  createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

  createInfo.enabledExtensionCount = (uint32_t)requiredExtensions.size();
  createInfo.ppEnabledExtensionNames = requiredExtensions.data();

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
  } else {
    createInfo.enabledLayerCount = 0;

    createInfo.pNext = nullptr;
  }

  VkResult result = vkCreateInstance(&createInfo, nullptr, &vulkan_instance);

  if (result != VK_SUCCESS) {
    std::stringstream ss;
    ss << "failed to create instance. code: " << (int)result;
    Logger::Log(LogLevel_Error, ss.str(), __FILE__, __LINE__);
    exit(-1);
  }

  Logger::Log(LogLevel_Info, "vulkan intialized successfully.", __FILE__,
              __LINE__);
}
