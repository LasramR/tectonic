#include "ttn_vertex_buffer.hpp"

#include <stdexcept>
#include <optional>
#include <cstring>

Ttn::vertex::Ttn_Vertex_Buffer::Ttn_Vertex_Buffer(VkPhysicalDevice vkPhysicalDevice, VkDevice vkDevice, Ttn::vertex::TtnVertex ttnVertex) :
  vkDevice{vkDevice},
  vertexBufferSize{static_cast<uint32_t>(ttnVertex.vertices.size())}
{
  VkBufferCreateInfo bufferInfo {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = sizeof(Ttn::vertex::Vertex) * ttnVertex.vertices.size();
  bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(vkDevice, &bufferInfo, nullptr, &this->vertexBuffer) != VK_SUCCESS) {
    throw std::runtime_error("could not create vertex buffer");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(vkDevice, this->vertexBuffer, &memRequirements);

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memProperties);

  VkMemoryAllocateInfo memAllocInfo {};
  memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memAllocInfo.allocationSize = memRequirements.size;
  memAllocInfo.memoryTypeIndex = this->findMemoryType(
    memProperties,
    memRequirements.memoryTypeBits,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
  );

  if (vkAllocateMemory(vkDevice, &memAllocInfo, nullptr, &this->deviceMemory) != VK_SUCCESS) {
    throw std::runtime_error("could not allocate buffer memory");
  }
  vkBindBufferMemory(vkDevice, this->vertexBuffer, this->deviceMemory, 0);
  
  void* data;
  vkMapMemory(this->vkDevice, this->deviceMemory, 0, bufferInfo.size, 0, &data);
  memcpy(data, ttnVertex.vertices.data(), (size_t) bufferInfo.size);
  vkUnmapMemory(this->vkDevice, this->deviceMemory);
}

Ttn::vertex::Ttn_Vertex_Buffer::~Ttn_Vertex_Buffer() {
  vkDestroyBuffer(this->vkDevice, this->vertexBuffer, nullptr);
  vkFreeMemory(this->vkDevice, this->deviceMemory, nullptr);
}

uint32_t Ttn::vertex::Ttn_Vertex_Buffer::findMemoryType(VkPhysicalDeviceMemoryProperties memProperties, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
  std::optional<uint32_t> memoryType;
  for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
      if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
          memoryType = i;
      }
  }

  if (!memoryType.has_value()) {
    throw std::runtime_error("failed to find suitable memory type!");
  }

  return memoryType.value();
}
