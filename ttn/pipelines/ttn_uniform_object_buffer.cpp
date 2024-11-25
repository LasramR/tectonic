#include "ttn_uniform_object_buffer.hpp"

#include <stdexcept>
#include <array>

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

  VkDescriptorSetLayoutBinding samplerLayoutBinding{};
  samplerLayoutBinding.binding = 1;
  samplerLayoutBinding.descriptorCount = 1;
  samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerLayoutBinding.pImmutableSamplers = nullptr;
  samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
  VkDescriptorSetLayoutCreateInfo baseLayoutInfo {};
  baseLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  baseLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
  baseLayoutInfo.pBindings = bindings.data();

  if (vkCreateDescriptorSetLayout(this->vkDevice, &baseLayoutInfo, nullptr, &this->layoutDescriptor) != VK_SUCCESS) {
    throw std::runtime_error("could not create ubo layout descriptor");
  }

  std::array<VkDescriptorPoolSize, 2> poolSizes{};
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = static_cast<uint32_t>(this->maxFrameInFlight);
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = static_cast<uint32_t>(this->maxFrameInFlight);

  VkDescriptorPoolCreateInfo poolInfo {};
  poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolInfo.poolSizeCount = poolSizes.size();
  poolInfo.pPoolSizes = poolSizes.data();
  poolInfo.maxSets = static_cast<uint32_t>(this->maxFrameInFlight);
  
  if (vkCreateDescriptorPool(this->vkDevice, &poolInfo, nullptr, &this->descriptorPool) != VK_SUCCESS) {
    throw std::runtime_error("could not create descriptor pool");
  }
}

Ttn::pipelines::Ttn_UniformObjectBuffer::~Ttn_UniformObjectBuffer() {
  vkDestroyDescriptorPool(this->vkDevice, this->descriptorPool, nullptr);
  vkDestroyDescriptorSetLayout(this->vkDevice, this->layoutDescriptor, nullptr);
}