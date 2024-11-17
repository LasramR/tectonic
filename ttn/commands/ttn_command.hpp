#pragma once

#include <ttn/devices/ttn_logical_device.hpp>
#include <ttn/devices/ttn_physical_device.hpp>
#include <ttn/graphics/ttn_framebuffer.hpp>
#include <ttn/pipelines/ttn_renderpass.hpp>
#include <ttn/swapchain/ttn_swapchain.hpp>
#include <ttn/pipelines/ttn_graphic_pipeline.hpp>

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace commands {

    class Ttn_Command {

      private:
        Ttn::devices::Ttn_Logical_Device& ttnLogicalDevice;
        Ttn::devices::Ttn_Physical_Device& ttnPhysicalDevice;
        Ttn::graphics::Ttn_Framebuffer& ttnFramebuffer;
        Ttn::pipelines::Ttn_Renderpass& ttnRenderpass;
        Ttn::swapchain::Ttn_SwapChain& ttnSwapChain;
        Ttn::pipelines::Ttn_Graphic_Pipeline& ttnGraphicPipeline;
        
        VkCommandPool commandPool;
        const int commandBuffersCount;
        std::vector<VkCommandBuffer> commandBuffers;
      
      public:
        Ttn_Command(Ttn::devices::Ttn_Logical_Device&, Ttn::devices::Ttn_Physical_Device&, Ttn::graphics::Ttn_Framebuffer&, Ttn::pipelines::Ttn_Renderpass&, Ttn::swapchain::Ttn_SwapChain&, Ttn::pipelines::Ttn_Graphic_Pipeline&, const int);
        ~Ttn_Command();

        void recordCommandBuffer(uint32_t, uint32_t);
        void resetCommandBuffer(uint32_t);
        VkResult submitCommandBuffer(uint32_t, uint32_t, VkSemaphore, VkSemaphore, VkFence);
    };

  };

};