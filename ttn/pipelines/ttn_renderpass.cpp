#include "ttn_renderpass.hpp"

#include <stdexcept>

Ttn::pipelines::Ttn_Renderpass::Ttn_Renderpass(VkDevice vkDevice, VkFormat swapChainImageFormat) :
  vkDevice{vkDevice},
  swapChainImageFormat{swapChainImageFormat}
{
  VkAttachmentDescription colorAttachment {};
  colorAttachment.format = this->swapChainImageFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderpassCreateInfo {};
  renderpassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderpassCreateInfo.attachmentCount = 1;
  renderpassCreateInfo.pAttachments = &colorAttachment;
  renderpassCreateInfo.subpassCount = 1;
  renderpassCreateInfo.pSubpasses = &subpass;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  renderpassCreateInfo.dependencyCount = 1;
  renderpassCreateInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(this->vkDevice, &renderpassCreateInfo, nullptr, &this->renderpass) != VK_SUCCESS) {
    throw std::runtime_error("could not create render pass");
  }
}

Ttn::pipelines::Ttn_Renderpass::~Ttn_Renderpass() {
  vkDestroyRenderPass(this->vkDevice, this->renderpass, nullptr);
}

VkRenderPass Ttn::pipelines::Ttn_Renderpass::getRenderpass() {
  return this->renderpass;
}
