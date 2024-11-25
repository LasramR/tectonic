#include "ttn_depth.hpp"

#include <stdexcept>

Ttn::depth::Ttn_Depth::Ttn_Depth(VkDevice vkDevice, Ttn::devices::Ttn_Physical_Device& ttnPhysicalDevice, Ttn::swapchain::Ttn_SwapChain& ttnSwapChain, Ttn::vertex::Ttn_Vertex_Buffer& ttnVertexBuffer, Ttn::commands::Ttn_Command& ttnCommand) :
  vkDevice {vkDevice},
  ttnPhysicalDevice {ttnPhysicalDevice},
  ttnSwapChain {ttnSwapChain},
  ttnVertexBuffer {ttnVertexBuffer},
  ttnCommand {ttnCommand}
{
  auto depthFormat = this->ttnPhysicalDevice.findDepthFormat();
  this->createImage(this->ttnSwapChain.getSwapChainExtent().width, this->ttnSwapChain.getSwapChainExtent().height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->depthImage, this->depthImageMemory);

  VkImageViewCreateInfo viewInfo{};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.image = this->depthImage;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = depthFormat;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = 1;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;

  if (vkCreateImageView(this->vkDevice, &viewInfo, nullptr, &this->depthImageView) != VK_SUCCESS) {
    throw std::runtime_error("could not create depth image view");
  }

  this->ttnCommand.transitionImageLayout(this->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

Ttn::depth::Ttn_Depth::~Ttn_Depth() {
  vkDestroyImageView(this->vkDevice, this->depthImageView, nullptr);
  vkDestroyImage(this->vkDevice, this->depthImage, nullptr);
  vkFreeMemory(this->vkDevice, this->depthImageMemory, nullptr);
}



void Ttn::depth::Ttn_Depth::createImage(uint32_t width, uint32_t height, VkFormat imageFormat, VkImageTiling imageTiling, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags properties, VkImage& vkImage, VkDeviceMemory& vkMemory) {
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
  vkGetPhysicalDeviceMemoryProperties(this->ttnPhysicalDevice.GetVkPhysicalDevice(), &memProperties);
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
