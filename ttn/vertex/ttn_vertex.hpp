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

      bool operator==(const Vertex& other) const;
    } Vertex;

    class TtnVertex {

      public:
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        TtnVertex(const std::vector<Vertex>, const std::vector<uint32_t>);
        TtnVertex(const char*);
        ~TtnVertex();
        static TtnVertex Default();

        VkVertexInputBindingDescription getBindingDescription();        
        std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
    };
  }

}