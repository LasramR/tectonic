#include "ttn_image_view.hpp"

#include <stdexcept>
#include <iostream>
#include <ttn/shared/image.hpp>

Ttn::swapchain::Ttn_Image_View::Ttn_Image_View(VkDevice vkDevice, Ttn::swapchain::Ttn_SwapChain* ttnSwapChain) :
  vkDevice{vkDevice},
  ttnSwapChain{ttnSwapChain}
{
  auto swapChainImages = this->ttnSwapChain->getImageBuffer();
  
  this->imageViews.resize(swapChainImages->size());
  for (int i = 0; i < swapChainImages->size(); i++) {
    Ttn::shared::createImageView(this->vkDevice, swapChainImages->data()[i], this->ttnSwapChain->getSwapChainFormat(), VK_IMAGE_ASPECT_COLOR_BIT, 1, true, this->imageViews.data()[i]);
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