#pragma once

#include <ttn/devices/ttn_logical_device.hpp>
#include <ttn/devices/ttn_physical_device.hpp>
#include <ttn/graphics/ttn_framebuffer.hpp>
#include <ttn/pipelines/ttn_renderpass.hpp>
#include <ttn/swapchain/ttn_swapchain.hpp>
#include <ttn/pipelines/ttn_graphic_pipeline.hpp>
#include <ttn/vertex/ttn_vertex_buffer.hpp>

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace commands {

    class Ttn_Command {

      private:
        Ttn::devices::Ttn_Logical_Device& ttnLogicalDevice;
        Ttn::devices::Ttn_Physical_Device& ttnPhysicalDevice;
        Ttn::graphics::Ttn_Framebuffer* ttnFramebuffer;
        Ttn::pipelines::Ttn_Renderpass* ttnRenderpass;
        Ttn::swapchain::Ttn_SwapChain& ttnSwapChain;
        Ttn::pipelines::Ttn_Graphic_Pipeline* ttnGraphicPipeline;
        Ttn::vertex::Ttn_Vertex_Buffer& ttnVertexBuffer;

        VkCommandPool commandPool;
        const int commandBuffersCount;
        std::vector<VkCommandBuffer> commandBuffers;

        bool hasStencilComponent(VkFormat);

        VkCommandBuffer beginSingleTimeCommand();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
      
      public:
        Ttn_Command(Ttn::devices::Ttn_Logical_Device&, Ttn::devices::Ttn_Physical_Device&, Ttn::swapchain::Ttn_SwapChain&, const int, Ttn::vertex::Ttn_Vertex_Buffer&);
        ~Ttn_Command();

        void bindGraphicPipeline(Ttn::pipelines::Ttn_Graphic_Pipeline* ttnGraphicPipeline);
        void bindFramebuffer(Ttn::graphics::Ttn_Framebuffer* ttnFramebuffer);
        void bindRenderpass(Ttn::pipelines::Ttn_Renderpass* ttnRenderpass);
        void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);
        void copyBufferToImage(VkBuffer, VkImage, uint32_t, uint32_t);
        void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t);
        void recordCommandBuffer(uint32_t, uint32_t);
        void resetCommandBuffer(uint32_t);
        VkResult submitCommandBuffer(uint32_t, uint32_t, VkSemaphore, VkSemaphore, VkFence);
        void generateMipMaps(VkImage image, VkFormat imageFormat, uint32_t texWidth, uint32_t texHeight, uint32_t mipLevels);
    };

  };

};