#pragma once

#include <vector>

#include <ttn/vertex/ttn_vertex.hpp>

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace vertex {
    typedef struct StagingBuffer {
      VkBuffer vkBuffer;
      VkDeviceMemory vkDeviceMemory;
    } StagingBuffer;

    class Ttn_Vertex_Buffer {

      private:
        size_t maxFrameInFlight;
        VkDevice vkDevice;
        VkPhysicalDevice vkPhysicalDevice;
        uint32_t findMemoryType(VkPhysicalDeviceMemoryProperties, uint32_t, VkMemoryPropertyFlags);
        void createBuffer(VkDeviceSize, VkBufferUsageFlags, VkMemoryPropertyFlags, VkBuffer&, VkDeviceMemory&);
      public:
        VkDeviceSize indexBufferSize;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        VkDeviceSize bufferSize;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;

        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;
        std::vector<void*> uniformBufferMapped;

        Ttn::vertex::TtnVertex ttnVertex;

        Ttn_Vertex_Buffer(VkPhysicalDevice, VkDevice, Ttn::vertex::TtnVertex, size_t);
        ~Ttn_Vertex_Buffer();

        StagingBuffer* createStagingBuffer(const void*, VkDeviceSize);
        void createUniformBuffers(std::vector<VkBuffer>&, std::vector<VkDeviceMemory>&, std::vector<void*>&);
    };

  }

}