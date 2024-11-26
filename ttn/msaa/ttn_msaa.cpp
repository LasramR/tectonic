#include "ttn_msaa.hpp"

#include <stdexcept>

#include <ttn/shared/image.hpp>

Ttn::msaa::Ttn_Msaa::Ttn_Msaa(VkDevice vkDevice, Ttn::devices::Ttn_Physical_Device& ttnPhysicalDevice, Ttn::swapchain::Ttn_SwapChain& ttnSwapChain) :
  vkDevice {vkDevice},
  ttnPhysicalDevice {ttnPhysicalDevice},
  ttnSwapChain {ttnSwapChain}
{
  auto colorFormat = this->ttnSwapChain.getSwapChainFormat();

  Ttn::shared::createImage(
    this->vkDevice,
    this->ttnPhysicalDevice.GetVkPhysicalDevice(),
    this->ttnSwapChain.getSwapChainExtent().width,
    this->ttnSwapChain.getSwapChainExtent().height,
    1,
    this->ttnPhysicalDevice.msaaSamples,
    colorFormat,
    VK_IMAGE_TILING_OPTIMAL,
    VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
    this->colorImage,
    this->colorImageMemory
  );
  Ttn::shared::createImageView(this->vkDevice, this->colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1, false, this->colorImageView);
}

Ttn::msaa::Ttn_Msaa::~Ttn_Msaa() {
  vkDestroyImageView(this->vkDevice, this->colorImageView, nullptr);
  vkDestroyImage(this->vkDevice, this->colorImage, nullptr);
  vkFreeMemory(this->vkDevice, this->colorImageMemory, nullptr);
}
