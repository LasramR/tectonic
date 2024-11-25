#pragma once

#include <vector>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace Ttn {

  namespace vertex {

    typedef struct Vertex {
      glm::vec2 position;
      glm::vec3 color;
    } Vertex;

    class TtnVertex {

      public:
        const std::vector<Vertex> vertices;
        const std::vector<uint16_t> indices;
        TtnVertex(const std::vector<Vertex>, const std::vector<uint16_t>);
        ~TtnVertex();
        static TtnVertex Default();

        VkVertexInputBindingDescription getBindingDescription();        
        std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
    };
  }

}