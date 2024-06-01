#include "lumina.hpp"
#include "util/log.hpp"

bool Lumina::checkValidationLayerSupport() {
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  std::unordered_set<std::string> availableLayerNames;
  for (const auto &layer : availableLayers) {
    availableLayerNames.insert(std::string(layer.layerName));
  }

  for (const char *layerName : validationLayers) {
    if (availableLayerNames.count(layerName) == 0) {
      return false;
    }
  }

  return true;
}
