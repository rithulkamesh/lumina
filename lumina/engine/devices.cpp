#include "lumina.hpp"
#include "util/log.hpp"
#include <cstdio>
#include <set>

bool Lumina::isDeviceSupported(VkPhysicalDevice device) {

  // vkGetPhysicalDeviceProperties(device, &deviceProperties);
  // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  // return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
  // &&
  //        deviceFeatures.geometryShader;

  QueueFamilyIndices indices = findQueueFamilies(device);
  return indices.isComplete();

  // return true;
}

void Lumina::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(vulkan_instance, &deviceCount, nullptr);

  Logger::Log(LogLevel_Info, "picking physical device", __FILE__, __LINE__);

  if (deviceCount == 0) {
    Logger::Log(LogLevel_Error, "no vulkan supported devices found.", __FILE__,
                __LINE__);
    exit(-1);
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(vulkan_instance, &deviceCount, devices.data());

  for (const auto &device : devices) {
    if (isDeviceSupported(device)) {
      physicalDevice = device;
      break;
    }
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    Logger::Log(LogLevel_Error, "no supported gpus found.", __FILE__, __LINE__);
    exit(-1);
  }
}

QueueFamilyIndices Lumina::findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {
    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, window_surface,
                                         &presentSupport);

    if (presentSupport) {
      indices.presentFamily = i;
    }

    if (indices.isComplete())
      break;

    i++;
  }

  return indices;
}

void Lumina::pickLogicalDevice() {
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
  VkPhysicalDeviceFeatures deviceFeatures{};
  VkDeviceCreateInfo createInfo{};
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  float queuePriority = 1.0f;

  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = 0;
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS) {

    Logger::Log(LogLevel_Error, "failed to create logical device.", __FILE__,
                __LINE__);
    exit(-1);
  }

  vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}
