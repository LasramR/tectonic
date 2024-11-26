#include "ttn_depth.hpp"

#include <stdexcept>

#include <ttn/shared/image.hpp>

Ttn::depth::Ttn_Depth::Ttn_Depth(VkDevice vkDevice, Ttn::devices::Ttn_Physical_Device& ttnPhysicalDevice, Ttn::swapchain::Ttn_SwapChain& ttnSwapChain, Ttn::vertex::Ttn_Vertex_Buffer& ttnVertexBuffer, Ttn::commands::Ttn_Command& ttnCommand) :
  vkDevice {vkDevice},
  ttnPhysicalDevice {ttnPhysicalDevice},
  ttnSwapChain {ttnSwapChain},
  ttnVertexBuffer {ttnVertexBuffer},
  ttnCommand {ttnCommand}
{
  auto depthFormat = this->ttnPhysicalDevice.findDepthFormat();
  Ttn::shared::createImage(this->vkDevice, this->ttnPhysicalDevice.GetVkPhysicalDevice(), this->ttnSwapChain.getSwapChainExtent().width, this->ttnSwapChain.getSwapChainExtent().height, 1, this->ttnPhysicalDevice.msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->depthImage, this->depthImageMemory);
  Ttn::shared::createImageView(this->vkDevice, this->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1, false, this->depthImageView);

  this->ttnCommand.transitionImageLayout(this->depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

Ttn::depth::Ttn_Depth::~Ttn_Depth() {
  vkDestroyImageView(this->vkDevice, this->depthImageView, nullptr);
  vkDestroyImage(this->vkDevice, this->depthImage, nullptr);
  vkFreeMemory(this->vkDevice, this->depthImageMemory, nullptr);
}
