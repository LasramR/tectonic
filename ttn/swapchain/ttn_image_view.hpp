#pragma once


#include <vector>

#include "ttn_swapchain.hpp"
#include "vulkan/vulkan.h"

namespace Ttn {

  namespace swapchain {

    class Ttn_Image_View {

      private:
        VkDevice vkDevice;
        Ttn_SwapChain* ttnSwapChain;
        std::vector<VkImageView> imageViews;

      public:
        Ttn_Image_View(VkDevice, Ttn_SwapChain*);
        ~Ttn_Image_View();

        std::vector<VkImageView>& getImageViews();
    };

  }
}