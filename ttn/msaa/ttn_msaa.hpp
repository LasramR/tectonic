#pragma once

#include <ttn/devices/ttn_physical_device.hpp>
#include <ttn/swapchain/ttn_swapchain.hpp>
#include <vulkan/vulkan.h>

namespace Ttn {

  namespace msaa {

    class Ttn_Msaa {

      private:
        VkDevice vkDevice;
        Ttn::devices::Ttn_Physical_Device& ttnPhysicalDevice;
        Ttn::swapchain::Ttn_SwapChain& ttnSwapChain;

      public:
        VkImage colorImage;
        VkDeviceMemory colorImageMemory;
        VkImageView colorImageView;

        Ttn_Msaa(VkDevice, Ttn::devices::Ttn_Physical_Device&, Ttn::swapchain::Ttn_SwapChain&);
        ~Ttn_Msaa();

    };

  }
}