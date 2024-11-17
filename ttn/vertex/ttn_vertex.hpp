#pragma once

#include <vector>
#include <array>

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
        TtnVertex(const std::vector<Vertex>);
        ~TtnVertex();
        static TtnVertex Default();

        VkVertexInputBindingDescription getBindingDescription();        
        std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
    };
  }

}