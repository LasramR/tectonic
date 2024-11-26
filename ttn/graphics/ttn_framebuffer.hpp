#pragma once

#include <vector>

#include <ttn/swapchain/ttn_swapchain.hpp>
#include <ttn/swapchain/ttn_image_view.hpp>
#include <ttn/pipelines/ttn_renderpass.hpp>
#include <vulkan/vulkan.h>

namespace Ttn {

  namespace graphics {

    class Ttn_Framebuffer {
      
      private:
        VkDevice vkDevice;
        Ttn::swapchain::Ttn_SwapChain& ttnSwapChain;
        Ttn::swapchain::Ttn_Image_View& ttnImageView;
        Ttn::pipelines::Ttn_Renderpass& ttnRenderpass;
        std::vector<VkFramebuffer> swapChainFrameBuffers;
        VkImageView depthImageView;

      public:
        Ttn_Framebuffer(VkDevice, Ttn::swapchain::Ttn_SwapChain&, Ttn::swapchain::Ttn_Image_View&, Ttn::pipelines::Ttn_Renderpass&, VkImageView, VkImageView);
        ~Ttn_Framebuffer();
        
        std::vector<VkFramebuffer>& getSwapChainFrameBuffers();
    };

  };

};