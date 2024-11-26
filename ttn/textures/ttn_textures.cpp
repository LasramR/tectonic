#include "ttn_textures.hpp"

#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Ttn::textures::Ttn_Texture::Ttn_Texture(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, const char* texturePath, Ttn::vertex::Ttn_Vertex_Buffer& ttnVertexBuffer, Ttn::commands::Ttn_Command& ttnCommand) :
  vkDevice {vkDevice},
  vkPhysicalDevice {vkPhysicalDevice},
  ttnVertexBuffer {ttnVertexBuffer},
  ttnCommand {ttnCommand},
  textureImage {VK_NULL_HANDLE}
{
  int texWidth, texHeight, texChannels;
  stbi_uc* pixels = stbi_load(texturePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
  
  if (!pixels) {
    throw std::runtime_error("could not load texture");
  }

  mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;

  VkDeviceSize imageSize = texWidth * texHeight * 4; // RGBA=4
  auto stagingBuffer = ttnVertexBuffer.createStagingBuffer(pixels, imageSize);
  stbi_image_free(pixels);

  Ttn::shared::createImage(this->vkDevice, this->vkPhysicalDevice, texWidth, texHeight, this->mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->textureImage, this->textureImageMemory);
  this->ttnCommand.transitionImageLayout(this->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, this->mipLevels);
  this->ttnCommand.copyBufferToImage(stagingBuffer->vkBuffer, this->textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
  this->ttnCommand.generateMipMaps(this->textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, this->mipLevels);
  
  vkDestroyBuffer(this->vkDevice, stagingBuffer->vkBuffer, nullptr);
  vkFreeMemory(this->vkDevice, stagingBuffer->vkDeviceMemory, nullptr);

  Ttn::shared::createImageView(this->vkDevice, this->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, this->mipLevels, false, this->textureImageView);
  
  this->ttnSampler = new Ttn::textures::Ttn_Sampler(this->vkDevice, this->vkPhysicalDevice, this->mipLevels);
}

Ttn::textures::Ttn_Texture::~Ttn_Texture() {
  delete this->ttnSampler;
  vkDestroyImageView(this->vkDevice, this->textureImageView, nullptr);
  vkDestroyImage(this->vkDevice, this->textureImage, nullptr);
  vkFreeMemory(this->vkDevice, this->textureImageMemory, nullptr);
}
