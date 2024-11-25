#pragma once

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace textures {

    class Ttn_Sampler {

      private:
        VkDevice vkDevice;
        VkPhysicalDevice vkPhysicalDevice;
      public:
        VkSampler textureSampler;

        Ttn_Sampler(VkDevice vkDevice, VkPhysicalDevice);
        ~Ttn_Sampler();

    };
  }
}