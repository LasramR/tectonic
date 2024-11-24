#include "ttn_vertex.hpp"

Ttn::vertex::TtnVertex::TtnVertex(std::vector<Ttn::vertex::Vertex> vertices, const std::vector<uint16_t> indices) :
  vertices{vertices},
  indices{indices}
{

}

Ttn::vertex::TtnVertex::~TtnVertex() {

}

Ttn::vertex::TtnVertex Ttn::vertex::TtnVertex::Default() {
  return Ttn::vertex::TtnVertex({
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
  }, {
    0, 1, 2, 2, 3, 0,
    4, 5, 6, 6, 7, 4
  });
}

VkVertexInputBindingDescription Ttn::vertex::TtnVertex::getBindingDescription() {
  VkVertexInputBindingDescription bindingDescription {};

  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Ttn::vertex::Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  
  return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 3> Ttn::vertex::TtnVertex::getAttributeDescriptions() {
  std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions {};

  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, position);

  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Vertex, color);


  attributeDescriptions[2].binding = 0;
  attributeDescriptions[2].location = 2;
  attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

  return attributeDescriptions;
}
