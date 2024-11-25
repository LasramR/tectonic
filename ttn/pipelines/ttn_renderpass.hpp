#pragma once

#include <vulkan/vulkan.h>

#include <ttn/devices/ttn_physical_device.hpp>

namespace Ttn {

  namespace pipelines {

    class Ttn_Renderpass {

      private:
        VkDevice vkDevice;
        VkFormat swapChainImageFormat;
        VkRenderPass renderpass;
        Ttn::devices::Ttn_Physical_Device& ttnPhysicalDevice;

      public:
        Ttn_Renderpass(VkDevice, VkFormat, Ttn::devices::Ttn_Physical_Device&);
        ~Ttn_Renderpass();

        VkRenderPass getRenderpass();
    };

  };
};