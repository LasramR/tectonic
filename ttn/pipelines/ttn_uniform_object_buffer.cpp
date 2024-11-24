#include "ttn_uniform_object_buffer.hpp"

#include <stdexcept>

Ttn::pipelines::Ttn_UniformObjectBuffer::Ttn_UniformObjectBuffer(VkDevice vkDevice, size_t maxFrameInFlight) :
  vkDevice{vkDevice},
  layoutDescriptor{VK_NULL_HANDLE},
  maxFrameInFlight{maxFrameInFlight}
{
  VkDescriptorSetLayoutBinding uboLayoutBinding {};
  uboLayoutBinding.binding = 0;
  uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uboLayoutBinding.descriptorCount = 1;
  uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  uboLayoutBinding.pImmutableSamplers = nullptr;

  VkDescriptorSetLayoutCreateInfo layoutInfo {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = &uboLayoutBinding;

  if (vkCreateDescriptorSetLayout(this->vkDevice, &layoutInfo, nullptr, &this->layoutDescriptor) != VK_SUCCESS) {
    throw std::runtime_error("could not create ubo layout descriptor");
  }

  VkDescriptorPoolSize poolSize {};
  poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = static_cast<uint32_t>(this->maxFrameInFlight);
  
  VkDescriptorPoolCreateInfo poolInfo {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = 1;
  poolInfo.pPoolSizes = &poolSize;
  poolInfo.maxSets = static_cast<uint32_t>(this->maxFrameInFlight);
  
  if (vkCreateDescriptorPool(this->vkDevice, &poolInfo, nullptr, &this->descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("could not create descriptor pool");
  }
}

Ttn::pipelines::Ttn_UniformObjectBuffer::~Ttn_UniformObjectBuffer() {
  vkDestroyDescriptorPool(this->vkDevice, this->descriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(this->vkDevice, this->layoutDescriptor, nullptr);
}