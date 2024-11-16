#pragma once

#include <vector>

#include "ttn_physical_device.hpp"
#include <ttn/shared/logger.hpp>

#include "vulkan/vulkan.h"

namespace Ttn {

  namespace devices {

    class Ttn_Logical_Device {
        
      private:
        VkInstance vkInstance;
        Ttn::devices::Ttn_Physical_Device* ttnPhysicalDevice;
        Ttn::Logger* logger;

        static constexpr float queuePriority = 1.0f;
        std::vector<VkDeviceQueueCreateInfo> queuesCreateInfo;
        VkQueue graphicsQueue;
        VkQueue presentQueue;
        VkDeviceCreateInfo vkDeviceCreateInfo;
        const std::vector<const char*>& validationLayers;
        VkDevice vkDevice;


      public:
        Ttn_Logical_Device(VkInstance, Ttn::devices::Ttn_Physical_Device*, Ttn::Logger*, const std::vector<const char*>&);
        ~Ttn_Logical_Device();

        VkDevice getDevice();
        VkQueue getGraphicsQueue();
        VkQueue getPresentQueue();
    };
  };
}