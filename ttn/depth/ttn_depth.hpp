#pragma once

#include <vector>

#include <ttn/devices/ttn_physical_device.hpp>
#include <ttn/swapchain/ttn_swapchain.hpp>
#include <ttn/vertex/ttn_vertex_buffer.hpp>
#include <ttn/commands/ttn_command.hpp>

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace depth {

    class Ttn_Depth {

      private:
        VkDevice vkDevice;
        Ttn::devices::Ttn_Physical_Device& ttnPhysicalDevice;
        Ttn::swapchain::Ttn_SwapChain& ttnSwapChain;
        Ttn::vertex::Ttn_Vertex_Buffer& ttnVertexBuffer;
        Ttn::commands::Ttn_Command& ttnCommand;

        void createImage(uint32_t width, uint32_t height, VkFormat imageFormat, VkImageTiling imageTiling, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags properties, VkImage& vkImage, VkDeviceMemory& vkMemory);

      public:
        VkImage depthImage;
        VkDeviceMemory depthImageMemory;
        VkImageView depthImageView;

        Ttn_Depth(VkDevice, Ttn::devices::Ttn_Physical_Device&, Ttn::swapchain::Ttn_SwapChain&, Ttn::vertex::Ttn_Vertex_Buffer&, Ttn::commands::Ttn_Command&);
        ~Ttn_Depth();

    };
  }
}