#pragma once

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace textures {

    class Ttn_Sampler {

      private:
        VkDevice vkDevice;
        VkPhysicalDevice vkPhysicalDevice;
        uint32_t mipLevels;
      public:
        VkSampler textureSampler;

        Ttn_Sampler(VkDevice vkDevice, VkPhysicalDevice, uint32_t);
        ~Ttn_Sampler();

    };
  }
}