#pragma once

#include <vector>

#include <ttn/shared/logger.hpp>
#include "vulkan/vulkan.h"

namespace Ttn {

  namespace devices {

    class Ttn_Physical_Device {

      private:
        VkInstance vkInstance;        
        Ttn::Logger& logger;

        uint32_t deviceCount;
        std::vector<VkPhysicalDevice> availablePhysicalDevices;
        
        VkPhysicalDeviceProperties vkPhysicalDeviceProperties;
        VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures;
        uint32_t physicalDeviceScore;
        VkPhysicalDevice vkPhysicalDevice;

        uint32_t getPhysicalDeviceScore(VkPhysicalDevice, VkPhysicalDeviceProperties, VkPhysicalDeviceFeatures);
      
      public:
        Ttn_Physical_Device(VkInstance, Ttn::Logger&);
        ~Ttn_Physical_Device();

    };

  };

}