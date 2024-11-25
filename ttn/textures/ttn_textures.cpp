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

  VkDeviceSize imageSize = texWidth * texHeight * 4; // RGBA=4
  auto stagingBuffer = ttnVertexBuffer.createStagingBuffer(pixels, imageSize);
  stbi_image_free(pixels);

  this->createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->textureImage, this->textureImageMemory);
  this->ttnCommand.transitionImageLayout(this->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  this->ttnCommand.copyBufferToImage(stagingBuffer->vkBuffer, this->textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
  this->ttnCommand.transitionImageLayout(this->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  vkDestroyBuffer(this->vkDevice, stagingBuffer->vkBuffer, nullptr);
  vkFreeMemory(this->vkDevice, stagingBuffer->vkDeviceMemory, nullptr);

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = textureImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(this->vkDevice, &viewInfo, nullptr, &this->textureImageView) != VK_SUCCESS) {
    throw std::runtime_error("could not create image view");
  }

  this->ttnSampler = new Ttn::textures::Ttn_Sampler(this->vkDevice, this->vkPhysicalDevice);
}

Ttn::textures::Ttn_Texture::~Ttn_Texture() {
  delete this->ttnSampler;
  vkDestroyImageView(this->vkDevice, this->textureImageView, nullptr);
  vkDestroyImage(this->vkDevice, this->textureImage, nullptr);
  vkFreeMemory(this->vkDevice, this->textureImageMemory, nullptr);
}

void Ttn::textures::Ttn_Texture::createImage(uint32_t width, uint32_t height, VkFormat imageFormat, VkImageTiling imageTiling, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags properties, VkImage& vkImage, VkDeviceMemory& vkMemory) {
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = 1;
  imageInfo.arrayLayers = 1;
  imageInfo.format = imageFormat;
  imageInfo.tiling = imageTiling;
  imageInfo.usage = imageUsage;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.flags = 0;


  if (vkCreateImage(this->vkDevice, &imageInfo, nullptr, &vkImage) != VK_SUCCESS) {
    throw std::runtime_error("could not create image");
  }

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(this->vkPhysicalDevice, &memProperties);
  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(this->vkDevice, vkImage, &memRequirements);

  VkMemoryAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = this->ttnVertexBuffer.findMemoryType(memProperties, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (vkAllocateMemory(this->vkDevice, &allocInfo, nullptr, &vkMemory) != VK_SUCCESS) {
    throw std::runtime_error("could not allocate image memory");
  }

  vkBindImageMemory(this->vkDevice, vkImage, vkMemory, 0);
}
