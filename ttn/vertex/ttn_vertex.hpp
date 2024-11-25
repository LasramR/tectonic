#pragma once

#include <vector>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace Ttn {

  namespace vertex {

    typedef struct Vertex {
      glm::vec3 position;
      glm::vec3 color;
      glm::vec2 texCoord;
    } Vertex;

    class TtnVertex {

      public:
        const std::vector<Vertex> vertices;
        const std::vector<uint16_t> indices;
        TtnVertex(const std::vector<Vertex>, const std::vector<uint16_t>);
        ~TtnVertex();
        static TtnVertex Default();

        VkVertexInputBindingDescription getBindingDescription();        
        std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
    };
  }

}