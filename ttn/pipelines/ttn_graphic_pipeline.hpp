#pragma once

#include <vector>

#include <ttn/shaders/ttn_shader.hpp>
#include <ttn/shared/logger.hpp>
#include <ttn/swapchain/ttn_swapchain.hpp>
#include <ttn/pipelines/ttn_renderpass.hpp>
#include <ttn/pipelines/ttn_uniform_object_buffer.hpp>
#include <ttn/vertex/ttn_vertex_buffer.hpp>

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace pipelines {

    class Ttn_Graphic_Pipeline {

      private:
        VkDevice vkDevice;
        const size_t maxFramesInFlight;
        Ttn::Logger& logger;
        Ttn::swapchain::Ttn_SwapChain& ttnSwapChain;
        Ttn::pipelines::Ttn_Renderpass& ttnRenderpass;
        Ttn::pipelines::Ttn_UniformObjectBuffer* ttnUbo;
        Ttn::vertex::Ttn_Vertex_Buffer& ttnVertexBuffer;

        std::vector<VkDynamicState> dynamicStates;

        VkPipeline pipeline;

      public:
        Ttn_Graphic_Pipeline(VkDevice, Ttn::Logger&, Ttn::swapchain::Ttn_SwapChain&, Ttn::pipelines::Ttn_Renderpass&, Ttn::vertex::Ttn_Vertex_Buffer&, VkImageView, VkSampler, size_t, Ttn::vertex::TtnVertex&);
        ~Ttn_Graphic_Pipeline();

        std::vector<VkDescriptorSet> descriptorSets;
        VkPipelineLayout pipelineLayout;
        
        VkPipeline getPipeline();
    };

  };

}