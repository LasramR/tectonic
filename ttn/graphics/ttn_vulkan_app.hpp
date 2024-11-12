#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "ttn_window.hpp"
#include "./../shared/logger.hpp"

#include <string>
#include <vector>

#include "vulkan/vulkan.h"

namespace Ttn {

  class VulkanApp {

    private:
    VkInstance vkInstance;
    VkApplicationInfo vkApplicationInfo;
    VkInstanceCreateInfo vkInstanceCreateInfo;

    uint32_t glfwExtensionCount;
    const char** glfwExtensions;

    uint32_t vkExtensionCount;
    std::vector<VkExtensionProperties> vkExtensions;

    Ttn_Window* window;
    Ttn::Logger& logger;

    public:
      VulkanApp(std::string name, Ttn::Logger& logger);
      ~VulkanApp();
      void initialize(Ttn::Ttn_WindowProperties windowProperties);
      void run();
      void cleanUp();
  };

}