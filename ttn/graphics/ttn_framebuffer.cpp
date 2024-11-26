#include "ttn_framebuffer.hpp"

#include <stdexcept>
#include <array>

Ttn::graphics::Ttn_Framebuffer::Ttn_Framebuffer(VkDevice vkDevice, Ttn::swapchain::Ttn_SwapChain& ttnSwapChain, Ttn::swapchain::Ttn_Image_View& ttnImageView, Ttn::pipelines::Ttn_Renderpass& ttnRenderpass, VkImageView depthImageView, VkImageView colorImageView) :
  vkDevice{vkDevice},
  ttnSwapChain{ttnSwapChain},
  ttnImageView{ttnImageView},
  ttnRenderpass{ttnRenderpass},
  depthImageView{depthImageView}
{
  this->swapChainFrameBuffers.resize(this->ttnImageView.getImageViews().size());
  for (size_t i = 0; i < this->ttnImageView.getImageViews().size(); i++) {
    std::array<VkImageView, 3> attachment = {
      // This order is actually important
      colorImageView,
      this->depthImageView,
      this->ttnImageView.getImageViews()[i]
    };

    VkFramebufferCreateInfo frameBufferCreateInfo {};
    frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferCreateInfo.renderPass = this->ttnRenderpass.getRenderpass();
    frameBufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachment.size());
    frameBufferCreateInfo.pAttachments = attachment.data();
    frameBufferCreateInfo.width = this->ttnSwapChain.getSwapChainExtent().width;
    frameBufferCreateInfo.height = this->ttnSwapChain.getSwapChainExtent().height;
    frameBufferCreateInfo.layers = 1;

    if (vkCreateFramebuffer(this->vkDevice, &frameBufferCreateInfo, nullptr, &this->swapChainFrameBuffers[i]) != VK_SUCCESS) {
      throw std::runtime_error("could not create frame buffer");
    }
  }
}

Ttn::graphics::Ttn_Framebuffer::~Ttn_Framebuffer() {
  for (auto frameBuffer : this->swapChainFrameBuffers) {
    vkDestroyFramebuffer(this->vkDevice, frameBuffer, nullptr);
  }
}

std::vector<VkFramebuffer>& Ttn::graphics::Ttn_Framebuffer::getSwapChainFrameBuffers() {
  return this->swapChainFrameBuffers;
}
