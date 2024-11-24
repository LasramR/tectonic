#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace Ttn {

  namespace pipelines {

    typedef struct {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    } UniformBufferObject;

    class Ttn_UniformObjectBuffer {
      private:
        VkDevice vkDevice;
        size_t maxFrameInFlight;
      public:
        VkDescriptorSetLayout layoutDescriptor;
        VkDescriptorPool descriptorPool;
        
        Ttn_UniformObjectBuffer(VkDevice, size_t);
        ~Ttn_UniformObjectBuffer();
    
    };
  }
}