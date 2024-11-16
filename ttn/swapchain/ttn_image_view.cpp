#include "ttn_image_view.hpp"

#include <stdexcept>
#include <iostream>

Ttn::swapchain::Ttn_Image_View::Ttn_Image_View(VkDevice vkDevice, Ttn::swapchain::Ttn_SwapChain* ttnSwapChain) :
  vkDevice{vkDevice},
  ttnSwapChain{ttnSwapChain}
{
  auto swapChainImages = this->ttnSwapChain->getImageBuffer();
  
  this->imageViews.resize(swapChainImages->size());
  for (int i = 0; i < swapChainImages->size(); i++) {
    VkImageViewCreateInfo imageViewCreateInfo {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = swapChainImages->data()[i];
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = this->ttnSwapChain->getSwapChainFormat();
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    if (vkCreateImageView(vkDevice, &imageViewCreateInfo, nullptr, &this->imageViews.data()[i]) != VK_SUCCESS) {
      throw std::runtime_error("failed to create image view");
    }
  }
}

Ttn::swapchain::Ttn_Image_View::~Ttn_Image_View() {
  for (const auto& imageView : this->imageViews) {
    vkDestroyImageView(this->vkDevice, imageView, nullptr);
  }
}

std::vector<VkImageView>& Ttn::swapchain::Ttn_Image_View::getImageViews() {
  return this->imageViews;
}