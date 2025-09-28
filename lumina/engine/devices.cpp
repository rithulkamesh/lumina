#include "lumina.hpp"
#include "util/log.hpp"
#include <cstdio>
#include <set>

bool Lumina::isDeviceSupported(VkPhysicalDevice device)
{
  Logger::Log(LogLevel_Info, "checking device support", __FILE__, __LINE__);

  // Log device properties for debugging
  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(device, &deviceProperties);
  Logger::Log(LogLevel_Info, "device name: " + std::string(deviceProperties.deviceName), __FILE__, __LINE__);

  const char *deviceTypeStr;
  switch (deviceProperties.deviceType)
  {
  case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
    deviceTypeStr = "integrated GPU";
    break;
  case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
    deviceTypeStr = "discrete GPU";
    break;
  case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    deviceTypeStr = "virtual GPU";
    break;
  case VK_PHYSICAL_DEVICE_TYPE_CPU:
    deviceTypeStr = "CPU";
    break;
  default:
    deviceTypeStr = "unknown";
    break;
  }
  Logger::Log(LogLevel_Info, "device type: " + std::string(deviceTypeStr), __FILE__, __LINE__);

  QueueFamilyIndices indices = findQueueFamilies(device);
  return indices.isComplete();
}

void Lumina::pickPhysicalDevice()
{
  Logger::Log(LogLevel_Info, "picking physical device", __FILE__, __LINE__);

  uint32_t deviceCount = 0;
  Logger::Log(LogLevel_Info, "enumerating physical devices (first call)", __FILE__, __LINE__);
  vkEnumeratePhysicalDevices(vulkan_instance, &deviceCount, nullptr);
  Logger::Log(LogLevel_Info, "enumeration complete, found " + std::to_string(deviceCount) + " device(s)", __FILE__, __LINE__);

  if (deviceCount == 0)
  {
    Logger::Log(LogLevel_Error, "no vulkan supported devices found.", __FILE__,
                __LINE__);
    exit(-1);
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  Logger::Log(LogLevel_Info, "enumerating physical devices (second call)", __FILE__, __LINE__);
  vkEnumeratePhysicalDevices(vulkan_instance, &deviceCount, devices.data());
  Logger::Log(LogLevel_Info, "enumeration data retrieval complete", __FILE__, __LINE__);

  Logger::Log(LogLevel_Info, "checking device suitability", __FILE__, __LINE__);
  for (const auto &device : devices)
  {
    Logger::Log(LogLevel_Info, "checking device", __FILE__, __LINE__);
    if (isDeviceSupported(device))
    {
      Logger::Log(LogLevel_Info, "found suitable device", __FILE__, __LINE__);
      physicalDevice = device;
      break;
    }
    Logger::Log(LogLevel_Info, "device not suitable, checking next", __FILE__, __LINE__);
  }

  if (physicalDevice == VK_NULL_HANDLE)
  {
    Logger::Log(LogLevel_Error, "no supported gpus found.", __FILE__, __LINE__);
    exit(-1);
  }
}

QueueFamilyIndices Lumina::findQueueFamilies(VkPhysicalDevice device)
{
  Logger::Log(LogLevel_Info, "finding queue families", __FILE__, __LINE__);
  QueueFamilyIndices indices;

  uint32_t queueFamilyCount = 0;
  Logger::Log(LogLevel_Info, "getting queue family count", __FILE__, __LINE__);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  Logger::Log(LogLevel_Info, "found " + std::to_string(queueFamilyCount) + " queue families", __FILE__, __LINE__);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  Logger::Log(LogLevel_Info, "getting queue family properties", __FILE__, __LINE__);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());
  Logger::Log(LogLevel_Info, "queue family properties retrieved", __FILE__, __LINE__);

  int i = 0;
  Logger::Log(LogLevel_Info, "iterating through queue families", __FILE__, __LINE__);
  for (const auto &queueFamily : queueFamilies)
  {
    Logger::Log(LogLevel_Info, "checking queue family " + std::to_string(i), __FILE__, __LINE__);

    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
    {
      Logger::Log(LogLevel_Info, "found graphics queue family at index " + std::to_string(i), __FILE__, __LINE__);
      indices.graphicsFamily = i;
    }

    Logger::Log(LogLevel_Info, "checking surface support for queue family " + std::to_string(i), __FILE__, __LINE__);
    VkBool32 presentSupport = false;
    VkResult surfaceResult = vkGetPhysicalDeviceSurfaceSupportKHR(device, i, window_surface, &presentSupport);
    if (surfaceResult != VK_SUCCESS)
    {
      Logger::Log(LogLevel_Error, "failed to check surface support for queue family " + std::to_string(i) + ", error: " + std::to_string(surfaceResult), __FILE__, __LINE__);
    }
    Logger::Log(LogLevel_Info, "surface support check complete for queue family " + std::to_string(i) + ", result: " + (presentSupport ? "supported" : "not supported"), __FILE__, __LINE__);

    if (presentSupport)
    {
      Logger::Log(LogLevel_Info, "found present queue family at index " + std::to_string(i), __FILE__, __LINE__);
      indices.presentFamily = i;
    }

    if (indices.isComplete())
    {
      Logger::Log(LogLevel_Info, "queue families complete", __FILE__, __LINE__);
      break;
    }

    i++;
  }

  return indices;
}

void Lumina::pickLogicalDevice()
{
  Logger::Log(LogLevel_Info, "creating logical device", __FILE__, __LINE__);
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
  Logger::Log(LogLevel_Info, "queue family indices retrieved", __FILE__, __LINE__);

  VkPhysicalDeviceFeatures deviceFeatures{};
  VkDeviceCreateInfo createInfo{};
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  Logger::Log(LogLevel_Info, "setting up queue create infos", __FILE__, __LINE__);
  float queuePriority = 1.0f;

  for (uint32_t queueFamily : uniqueQueueFamilies)
  {
    Logger::Log(LogLevel_Info, "creating queue info for family " + std::to_string(queueFamily), __FILE__, __LINE__);
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  Logger::Log(LogLevel_Info, "configuring device create info", __FILE__, __LINE__);
  createInfo.pEnabledFeatures = &deviceFeatures;

  createInfo.enabledExtensionCount = 0;
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  if (enableValidationLayers)
  {
    Logger::Log(LogLevel_Info, "enabling validation layers for device", __FILE__, __LINE__);
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  }
  else
  {
    createInfo.enabledLayerCount = 0;
  }

  Logger::Log(LogLevel_Info, "calling vkCreateDevice", __FILE__, __LINE__);
  VkResult deviceResult = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
  if (deviceResult != VK_SUCCESS)
  {
    Logger::Log(LogLevel_Error, "failed to create logical device. Error code: " + std::to_string(deviceResult), __FILE__, __LINE__);
    exit(-1);
  }
  Logger::Log(LogLevel_Info, "logical device created successfully", __FILE__, __LINE__);

  Logger::Log(LogLevel_Info, "getting device queue", __FILE__, __LINE__);
  vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
  Logger::Log(LogLevel_Info, "device queue retrieved successfully", __FILE__, __LINE__);
}
