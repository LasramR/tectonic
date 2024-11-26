#pragma once

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace shared {
    uint32_t findMemoryType(VkPhysicalDeviceMemoryProperties, uint32_t, VkMemoryPropertyFlags);
    void createImage(VkDevice, VkPhysicalDevice, uint32_t, uint32_t, uint32_t, VkSampleCountFlagBits, VkFormat, VkImageTiling, VkImageUsageFlags, VkMemoryPropertyFlags, VkImage&, VkDeviceMemory&);
    void createImageView(VkDevice, VkImage, VkFormat, VkImageAspectFlags, uint32_t, bool, VkImageView&);
  }
}