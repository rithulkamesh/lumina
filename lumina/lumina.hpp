#pragma once

#include <GLFW/glfw3.h>
#include <string>
#include <unordered_set>
#include <vulkan/vulkan.hpp>

const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};

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
  void MainLoop();

  void create_vulkan_instance();
  void get_supported_extensions();

  bool checkValidationLayerSupport();
};
