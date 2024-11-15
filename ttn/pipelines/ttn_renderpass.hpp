#pragma once

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace pipelines {

    class Ttn_Renderpass {

      private:
        VkDevice vkDevice;
        VkFormat swapChainImageFormat;
        VkRenderPass renderpass;

      public:
        Ttn_Renderpass(VkDevice, VkFormat);
        ~Ttn_Renderpass();

        VkRenderPass getRenderpass();
    };

  };
};