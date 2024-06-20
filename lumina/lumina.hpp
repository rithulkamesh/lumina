#pragma once

#include "util/log.hpp"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <vulkan/vulkan.hpp>

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

using namespace std;
typedef struct {
  float x;
  float y;
} Vector2;

typedef struct {
  Vector2 size;
  string title;
} WindowOptions;

typedef struct {
  int version[3];
  string identifier;
} GameMeta;

struct QueueFamilyIndices {
  std::optional<uint32_t> graphicsFamily;
  std::optional<uint32_t> presentFamily;

  bool isComplete() {
    return graphicsFamily.has_value() && presentFamily.has_value();
  }
};

class Lumina {
public:
  WindowOptions window_options;
  GameMeta game_metadata;
  /*
   * Run the game engine
   * Returns:
   * 0 if successful
   * 1 if internal error
   * 2 if external error
   */
  int Run();

private:
  GLFWwindow *window;
  VkInstance vulkan_instance;

  void Init();
  void Cleanup();

  void create_vulkan_instance();
  void get_supported_extensions();

  bool checkValidationLayerSupport();
  std::vector<const char *> getRequiredExtensions();

  // Fancy errors for the sake of it
  VkDebugUtilsMessengerEXT debugMessenger;
  void setupDebugMessenger();
  void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                     VkDebugUtilsMessengerEXT debugMessenger,
                                     const VkAllocationCallbacks *pAllocator);
  void populateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  // Okay actual gpu stuff now

  VkDevice device;
  VkQueue graphicsQueue; // vkGetDeviceQueue(device,
                         // indices.graphicsFamily.value(), 0, &graphicsQueue);
                         // Use the above function to get the device queue
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkPhysicalDeviceProperties deviceProperties;
  VkPhysicalDeviceFeatures deviceFeatures;

  void pickPhysicalDevice();
  void pickLogicalDevice();
  bool isDeviceSupported(VkPhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

  // Window Surface because platform agnosticity
  VkSurfaceKHR window_surface;
  VkQueue presentQueue;

  void createSurface();
};
