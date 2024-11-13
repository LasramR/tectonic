#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <ttn/graphics/ttn_window.hpp>
#include <ttn/shared/logger.hpp>
#include <ttn/debug/ttn_vulkan_debugger.hpp>

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
    std::vector<const char*> glfwExtensions;

    uint32_t vkExtensionCount;
    std::vector<VkExtensionProperties> vkExtensions;

    static const std::vector<const char*> vkValidationLayers;

    #ifdef NDEBUG
      static constexpr bool vkEnableValidationLayers = false;
    #else
      static constexpr bool vkEnableValidationLayers = true;
    #endif

    Ttn_Window* window;
    Ttn::Logger& logger;
    
    uint32_t vkAvailableValidationLayerCount;
    std::vector<VkLayerProperties> vkAvailableValidationLayerProperties;
    bool checkValidationLayerSupport();

    Ttn::debug::Vulkan_Debugger* vkDebugger;

    public:
      VulkanApp(std::string name, Ttn::Logger& logger);
      ~VulkanApp();
      void initialize(Ttn::Ttn_WindowProperties windowProperties);
      void run();
      void cleanUp();
  };

}