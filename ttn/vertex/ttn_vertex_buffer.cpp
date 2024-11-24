#include "ttn_vertex_buffer.hpp"

#include <ttn/pipelines/ttn_uniform_object_buffer.hpp>

#include <stdexcept>
#include <optional>
#include <cstring>

#include <iostream>

Ttn::vertex::Ttn_Vertex_Buffer::Ttn_Vertex_Buffer(VkPhysicalDevice vkPhysicalDevice, VkDevice vkDevice, Ttn::vertex::TtnVertex ttnVertex, size_t maxFrameInflight) :
  vkDevice{vkDevice},
  vkPhysicalDevice{vkPhysicalDevice},
  ttnVertex{ttnVertex},
  vertexBuffer{VK_NULL_HANDLE},
  maxFrameInFlight{maxFrameInflight}
{
  this->indexBufferSize = sizeof(this->ttnVertex.indices[0]) * this->ttnVertex.indices.size();
  this->createBuffer(this->indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->indexBuffer, this->indexBufferMemory);

  this->bufferSize = sizeof(this->ttnVertex.vertices[0]) * this->ttnVertex.vertices.size();
  this->createBuffer(this->bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->vertexBuffer, this->vertexBufferMemory);

  this->createUniformBuffers(this->uniformBuffers, this->uniformBuffersMemory, this->uniformBufferMapped);
}

Ttn::vertex::Ttn_Vertex_Buffer::~Ttn_Vertex_Buffer() {
  for (size_t i = 0; i < this->maxFrameInFlight; i++) {
    vkDestroyBuffer(this->vkDevice, this->uniformBuffers[i], nullptr);
    vkFreeMemory(this->vkDevice, this->uniformBuffersMemory[i], nullptr);
  }
  vkDestroyBuffer(this->vkDevice, this->indexBuffer, nullptr);
  vkFreeMemory(this->vkDevice, this->indexBufferMemory, nullptr);
  vkDestroyBuffer(this->vkDevice, this->vertexBuffer, nullptr);
  vkFreeMemory(this->vkDevice, this->vertexBufferMemory, nullptr);
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

void Ttn::vertex::Ttn_Vertex_Buffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& vkBuffer, VkDeviceMemory& vkBufferMemory) {
  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(this->vkPhysicalDevice, &memProperties);

  VkBufferCreateInfo bufferInfo {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  if (vkCreateBuffer(this->vkDevice, &bufferInfo, nullptr, &vkBuffer) != VK_SUCCESS) {
    throw std::runtime_error("could not create vertex buffer");
  }

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(this->vkDevice, vkBuffer, &memRequirements);

  VkMemoryAllocateInfo memAllocInfo {};
  memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memAllocInfo.allocationSize = memRequirements.size;
  memAllocInfo.memoryTypeIndex = this->findMemoryType(
    memProperties,
    memRequirements.memoryTypeBits,
    properties
  );

  if (vkAllocateMemory(this->vkDevice, &memAllocInfo, nullptr, &vkBufferMemory) != VK_SUCCESS) {
    throw std::runtime_error("could not allocate buffer memory");
  }

  vkBindBufferMemory(this->vkDevice, vkBuffer, vkBufferMemory, 0);
}


Ttn::vertex::StagingBuffer* Ttn::vertex::Ttn_Vertex_Buffer::createStagingBuffer(const void* data, VkDeviceSize size) {
  VkBuffer stagingBuffer = VK_NULL_HANDLE;
  VkDeviceMemory stagingMemory = VK_NULL_HANDLE;

  this->createBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);

  void* stagingData;
  vkMapMemory(this->vkDevice, stagingMemory, 0, size, 0, &stagingData);
  memcpy(stagingData, data, (size_t) size);
  vkUnmapMemory(this->vkDevice, stagingMemory);

  return new Ttn::vertex::StagingBuffer{
    vkBuffer: stagingBuffer,
    vkDeviceMemory: stagingMemory
  };
}

void Ttn::vertex::Ttn_Vertex_Buffer::createUniformBuffers(std::vector<VkBuffer>& buffers, std::vector<VkDeviceMemory>& memories, std::vector<void*>& dataVec) {
  VkDeviceSize size = sizeof(Ttn::pipelines::UniformBufferObject);
  buffers.resize(this->maxFrameInFlight);
  memories.resize(this->maxFrameInFlight);
  dataVec.resize(this->maxFrameInFlight);

  for (size_t i = 0; i < this->maxFrameInFlight; i++) {
    this->createBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffers[i], memories[i]);
    vkMapMemory(this->vkDevice, memories[i], 0, size, 0, &dataVec[i]);
  }
}