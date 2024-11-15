#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <ttn/debug/ttn_vulkan_debugger.hpp>
#include <ttn/devices/ttn_physical_device.hpp>
#include <ttn/devices/ttn_logical_device.hpp>
#include <ttn/graphics/ttn_window.hpp>
#include <ttn/shared/logger.hpp>
#include <ttn/swapchain/ttn_swapchain.hpp>
#include <ttn/swapchain/ttn_image_view.hpp>

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
    static const std::vector<const char*> vkRequiredExtensions;

    uint32_t enabledExtensionsCount;
    std::vector<const char*> enabledExtensions;

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
    Ttn::devices::Ttn_Physical_Device* ttnPhysicalDevice;
    Ttn::devices::Ttn_Logical_Device* ttnLogicalDevice;
    Ttn::swapchain::Ttn_SwapChain* ttnSwapChain;
    Ttn::swapchain::Ttn_Image_View* ttnImageView;

    public:
      VulkanApp(std::string name, Ttn::Ttn_WindowProperties windowProperties, Ttn::Logger& logger);
      ~VulkanApp();
      void initialize();
      void run();
      void cleanUp();
  };

}