#pragma once

#include <ttn/vertex/ttn_vertex_buffer.hpp>
#include <ttn/commands/ttn_command.hpp>
#include <ttn/textures/ttn_sampler.hpp>
#include <ttn/shared/image.hpp>

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace textures {

    class Ttn_Texture {
      private:
        VkDevice vkDevice;
        VkPhysicalDevice vkPhysicalDevice;
        Ttn::vertex::Ttn_Vertex_Buffer& ttnVertexBuffer;
        Ttn::commands::Ttn_Command& ttnCommand;
      public:
        uint32_t mipLevels;
        VkImage textureImage;
        VkDeviceMemory textureImageMemory;
        VkImageView textureImageView;
        Ttn::textures::Ttn_Sampler* ttnSampler;

        Ttn_Texture(VkDevice, VkPhysicalDevice, const char*, Ttn::vertex::Ttn_Vertex_Buffer&, Ttn::commands::Ttn_Command&);
        ~Ttn_Texture();
    };
  }
}