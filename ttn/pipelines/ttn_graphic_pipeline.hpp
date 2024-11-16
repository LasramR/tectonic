#pragma once

#include <vector>

#include <ttn/shaders/ttn_shader.hpp>
#include <ttn/shared/logger.hpp>
#include <ttn/swapchain/ttn_swapchain.hpp>
#include <ttn/pipelines/ttn_renderpass.hpp>

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace pipelines {

    class Ttn_Graphic_Pipeline {

      private:
        VkDevice vkDevice;
        Ttn::Logger& logger;
        Ttn::swapchain::Ttn_SwapChain& ttnSwapChain;
        Ttn::pipelines::Ttn_Renderpass& ttnRenderpass;

        std::vector<VkDynamicState> dynamicStates;
        VkPipelineLayout pipelineLayout;
        VkPipeline pipeline;

      public:
        Ttn_Graphic_Pipeline(VkDevice, Ttn::Logger&, Ttn::swapchain::Ttn_SwapChain&, Ttn::pipelines::Ttn_Renderpass&);
        ~Ttn_Graphic_Pipeline();

        VkPipeline getPipeline();
    };

  };

}