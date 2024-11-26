#include "image.hpp"

#include <stdexcept>
#include <optional>

uint32_t Ttn::shared::findMemoryType(VkPhysicalDeviceMemoryProperties memProperties, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
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

void Ttn::shared::createImage(VkDevice vkDevice, VkPhysicalDevice vkPhysicalDevice, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties , VkImage& vkImage, VkDeviceMemory& vkMemory) {
  VkImageCreateInfo imageInfo{};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.extent.width = width;
  imageInfo.extent.height = height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = mipLevels;
  imageInfo.arrayLayers = 1;
  imageInfo.format = format;
  imageInfo.tiling = tiling;
  imageInfo.usage = usage;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageInfo.samples = numSamples;
  imageInfo.flags = 0;

  if (vkCreateImage(vkDevice, &imageInfo, nullptr, &vkImage) != VK_SUCCESS) {
    throw std::runtime_error("could not create image");
  }

  VkPhysicalDeviceMemoryProperties memProperties;
  vkGetPhysicalDeviceMemoryProperties(vkPhysicalDevice, &memProperties);
  VkMemoryRequirements memRequirements;
  vkGetImageMemoryRequirements(vkDevice, vkImage, &memRequirements);

  VkMemoryAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = Ttn::shared::findMemoryType(memProperties, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (vkAllocateMemory(vkDevice, &allocInfo, nullptr, &vkMemory) != VK_SUCCESS) {
    throw std::runtime_error("could not allocate image memory");
  }

  vkBindImageMemory(vkDevice, vkImage, vkMemory, 0);
}

void Ttn::shared::createImageView(VkDevice vkDevice, VkImage vkImage, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels, bool swapChainImageView, VkImageView& vkImageView) {
  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = vkImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.subresourceRange.aspectMask = aspectFlags;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = mipLevels;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (swapChainImageView) {
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
  }

  if (vkCreateImageView(vkDevice, &viewInfo, nullptr, &vkImageView) != VK_SUCCESS) {
    throw std::runtime_error("could not create image view");
  }
}