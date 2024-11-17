#include "ttn_swapchain.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

#include <GLFW/glfw3.h>

Ttn::swapchain::Ttn_SwapChain::Ttn_SwapChain(
  VkDevice vkDevice,
  const Ttn::devices::SwapChainSupportDetails& swapChainSupportDetails, 
  const Ttn::devices::QueueFamilyIndices& queueFamilyIndices,
  Ttn::Ttn_Window* ttnWindow
) :
  vkDevice{vkDevice},
  swapChainSupportDetails{swapChainSupportDetails},
  queueFamilyIndices{queueFamilyIndices},
  ttnWindow{ttnWindow},
  swapChainCreateInfo{},
  swapChain{VK_NULL_HANDLE}
{
  this->swapChainFormatMode = this->selectSurfaceFormatKHR(this->swapChainSupportDetails.formats);
  this->swapChainPresentMode = this->selectPresentModeKHR(this->swapChainSupportDetails.presentModes);
  this->swapExtent = this->selectSwapExtent(this->swapChainSupportDetails.capabilities);
  this->swapChainImageCount = this->swapChainSupportDetails.capabilities.minImageCount + 1;
  if (this->swapChainSupportDetails.capabilities.maxImageCount > 0 && this->swapChainImageCount > this->swapChainSupportDetails.capabilities.maxImageCount) {
    this->swapChainImageCount = this->swapChainSupportDetails.capabilities.maxImageCount;
  }

  this->swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  this->swapChainCreateInfo.surface = this->ttnWindow->getSurface();
  this->swapChainCreateInfo.minImageCount = this->swapChainImageCount;
  this->swapChainCreateInfo.imageFormat = this->swapChainFormatMode.format;
  this->swapChainCreateInfo.imageColorSpace = this->swapChainFormatMode.colorSpace;
  this->swapChainCreateInfo.imageExtent = this->swapExtent;
  this->swapChainCreateInfo.imageArrayLayers = 1;
  this->swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // VK_IMAGE_USAGE_TRANSFER_DST_BIT when using post processing

  uint32_t queueFamilyIndicesArray[] = {this->queueFamilyIndices.graphicsFamily.value(), this->queueFamilyIndices.presentFamily.value()};

  if (this->queueFamilyIndices.graphicsFamily != this->queueFamilyIndices.presentFamily) {
    this->swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    this->swapChainCreateInfo.queueFamilyIndexCount = 2;
    this->swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndicesArray;
  } else {
    this->swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    this->swapChainCreateInfo.queueFamilyIndexCount = 0;
    this->swapChainCreateInfo.pQueueFamilyIndices = nullptr;
  }

  this->swapChainCreateInfo.preTransform = this->swapChainSupportDetails.capabilities.currentTransform;
  this->swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  this->swapChainCreateInfo.presentMode = this->swapChainPresentMode;
  this->swapChainCreateInfo.clipped = VK_TRUE;
  this->swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

  if (vkCreateSwapchainKHR(this->vkDevice, &this->swapChainCreateInfo, nullptr, &this->swapChain) != VK_SUCCESS) {
    throw std::runtime_error("could not create swap chain");
  }

  vkGetSwapchainImagesKHR(this->vkDevice, this->swapChain, &this->swapChainImageCount, nullptr);
  this->imageBuffer.resize(this->swapChainImageCount);
  vkGetSwapchainImagesKHR(this->vkDevice, this->swapChain, &this->swapChainImageCount, this->imageBuffer.data());
  this->swapChainFormat = this->swapChainFormatMode.format;
}

Ttn::swapchain::Ttn_SwapChain::~Ttn_SwapChain() {
  vkDestroySwapchainKHR(this->vkDevice, this->swapChain, nullptr);
}

VkSurfaceFormatKHR Ttn::swapchain::Ttn_SwapChain::selectSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
  for (const auto& availableFormat : availableFormats) {
      if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
          return availableFormat;
      }
  }

  return availableFormats[0];
}

VkPresentModeKHR Ttn::swapchain::Ttn_SwapChain::selectPresentModeKHR(const std::vector<VkPresentModeKHR>& availablePresentModes) {
  for (const auto& availablePresentMode : availablePresentModes) {
      if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
          return availablePresentMode;
      }
  }

  return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Ttn::swapchain::Ttn_SwapChain::selectSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
  if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  }

  int width, height;
  glfwGetFramebufferSize(this->ttnWindow->getWindow(), &width, &height);
  VkExtent2D actualExtent = {
      static_cast<uint32_t>(width),
      static_cast<uint32_t>(height)
  };

  actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
  actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
  
  return actualExtent;
}

std::vector<VkImage>* Ttn::swapchain::Ttn_SwapChain::getImageBuffer() {
  return &this->imageBuffer;
}

VkFormat Ttn::swapchain::Ttn_SwapChain::getSwapChainFormat() {
  return this->swapChainFormat;
}

VkExtent2D Ttn::swapchain::Ttn_SwapChain::getSwapChainExtent() {
  return this->swapExtent;
}

VkSwapchainKHR Ttn::swapchain::Ttn_SwapChain::getSwapChain() {
  return this->swapChain;
}
