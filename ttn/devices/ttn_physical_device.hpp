#pragma once

#include <optional>
#include <vector>

#include <ttn/shared/logger.hpp>
#include "vulkan/vulkan.h"

namespace Ttn {

  namespace devices {

    typedef struct QueueFamilyIndices {
      std::optional<uint32_t> graphicsFamily;
      std::optional<uint32_t> presentFamily;
      VkQueueFamilyProperties queueFamilyProperties;
      bool isComplete();
    } QueueFamilyIndices;
    
    typedef struct SwapChainSupportDetails {
      VkSurfaceCapabilitiesKHR capabilities;
      std::vector<VkSurfaceFormatKHR> formats;
      std::vector<VkPresentModeKHR> presentModes;
    } SwapChainSupportDetails;
    
    class Ttn_Physical_Device {

      private:
        VkInstance vkInstance;        
        VkSurfaceKHR vkSurface;
        Ttn::Logger& logger;

        uint32_t deviceCount;
        std::vector<VkPhysicalDevice> availablePhysicalDevices;
        
        static const std::vector<const char*> vkRequiredDeviceExtensions;

        VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
        VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures;
        uint32_t physicalDeviceScore;
        VkPhysicalDevice vkPhysicalDevice;
        uint32_t getPhysicalDeviceScore(VkPhysicalDevice, VkPhysicalDeviceProperties, VkPhysicalDeviceFeatures);
        bool CheckDeviceExtensionSupport(VkPhysicalDevice);
  
        QueueFamilyIndices queueFamily;
        QueueFamilyIndices findQueueFamily(VkPhysicalDevice, VkPhysicalDeviceProperties);

        SwapChainSupportDetails swapChainSupportDetails;
        SwapChainSupportDetails querySwapChainSupportDetails(VkPhysicalDevice vkPhysicalDevice);
        
        VkFormat findSupportedFormat(const std::vector<VkFormat>&, VkImageTiling, VkFormatFeatureFlags);
      public:
        Ttn_Physical_Device(VkInstance, VkSurfaceKHR, Ttn::Logger&);
        ~Ttn_Physical_Device();

        QueueFamilyIndices getQueueFamilyIndices();
        VkPhysicalDevice GetVkPhysicalDevice();
        SwapChainSupportDetails getSwapChainSupportDetails();
        const std::vector<const char*>& getRequiredDeviceExtension();
        VkFormat findDepthFormat();
    };

  };

}