#pragma once

#include <vector>

#include <ttn/vertex/ttn_vertex.hpp>

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace vertex {

    class Ttn_Vertex_Buffer {

      private:
        VkDevice vkDevice;
        uint32_t findMemoryType(VkPhysicalDeviceMemoryProperties, uint32_t, VkMemoryPropertyFlags);

      public:
        uint32_t vertexBufferSize;
        VkBuffer vertexBuffer;
        VkDeviceMemory deviceMemory;

        Ttn_Vertex_Buffer(VkPhysicalDevice, VkDevice, Ttn::vertex::TtnVertex);
        ~Ttn_Vertex_Buffer();
    };

  }

}