#include "ttn_vertex.hpp"

Ttn::vertex::TtnVertex::TtnVertex(std::vector<Ttn::vertex::Vertex> vertices) :
  vertices{vertices}
{

}

Ttn::vertex::TtnVertex::~TtnVertex() {

}

Ttn::vertex::TtnVertex Ttn::vertex::TtnVertex::Default() {
  return Ttn::vertex::TtnVertex({
    {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
    {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
  });
}

VkVertexInputBindingDescription Ttn::vertex::TtnVertex::getBindingDescription() {
  VkVertexInputBindingDescription bindingDescription {};

  bindingDescription.binding = 0;
  bindingDescription.stride = sizeof(Ttn::vertex::Vertex);
  bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  
  return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 2> Ttn::vertex::TtnVertex::getAttributeDescriptions() {
  std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions {};

  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = 0;
  attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
  attributeDescriptions[0].offset = offsetof(Vertex, position);

  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = 1;
  attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  attributeDescriptions[1].offset = offsetof(Vertex, color);

  return attributeDescriptions;
}
