#include "ttn_sampler.hpp"

#include <stdexcept>

Ttn::textures::Ttn_Sampler::Ttn_Sampler(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice) :
  vkDevice {vkDevice},
  vkPhysicalDevice {vkPhysicalDevice}
{
  VkSamplerCreateInfo samplerInfo {};
  samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerInfo.magFilter = VK_FILTER_LINEAR;
  samplerInfo.minFilter = VK_FILTER_LINEAR;
  samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerInfo.anisotropyEnable = VK_TRUE;

  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);

  samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
  samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  samplerInfo.unnormalizedCoordinates = VK_FALSE;
  samplerInfo.compareEnable = VK_FALSE;
  samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
  samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerInfo.mipLodBias = 0.0f;
  samplerInfo.minLod = 0.0f;
  samplerInfo.maxLod = 0.0f;

  if (vkCreateSampler(this->vkDevice, &samplerInfo, nullptr, &this->textureSampler) != VK_SUCCESS) {
    throw std::runtime_error("could not create sampler");
  }
}

Ttn::textures::Ttn_Sampler::~Ttn_Sampler() {
  vkDestroySampler(this->vkDevice, textureSampler, nullptr);
}