#include "ttn_vertex.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
  
#include <stdexcept>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std {
    template<> struct hash<Ttn::vertex::Vertex> {
        size_t operator()(Ttn::vertex::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.position) ^
                   (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}

bool Ttn::vertex::Vertex::operator==(const Vertex& other) const {
    return position == other.position && color == other.color && texCoord == other.texCoord;
}

Ttn::vertex::TtnVertex::TtnVertex(std::vector<Ttn::vertex::Vertex> vertices, const std::vector<uint32_t> indices) :
  vertices{vertices},
  indices{indices}
{

}

Ttn::vertex::TtnVertex::TtnVertex(const char* modelPath)
{
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;

  std::string warn, err;


  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath)) {
    throw std::runtime_error(warn + err);
  }

  std::unordered_map<Ttn::vertex::Vertex, uint32_t> uniqueVertices {};

  for (const auto& shape : shapes) {
    for (const auto& index : shape.mesh.indices) {
        Ttn::vertex::Vertex vertex{};
        vertex.position = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2]
        };

        vertex.texCoord = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
        };

        vertex.color = {1.0f, 1.0f, 1.0f};
        
        if (uniqueVertices.count(vertex) == 0) {
            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
            vertices.push_back(vertex);
        }

        indices.push_back(uniqueVertices[vertex]);
    }
  }
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
