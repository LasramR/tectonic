#include "ttn_command.hpp"

#include <stdexcept>

Ttn::commands::Ttn_Command::Ttn_Command(Ttn::devices::Ttn_Logical_Device& ttnLogicalDevice, Ttn::devices::Ttn_Physical_Device& ttnPhysicalDevice, Ttn::graphics::Ttn_Framebuffer& ttnFramebuffer, Ttn::pipelines::Ttn_Renderpass& ttnRenderpass, Ttn::swapchain::Ttn_SwapChain& ttnSwapChain, Ttn::pipelines::Ttn_Graphic_Pipeline& ttnGraphicPipeline) :
  ttnLogicalDevice{ttnLogicalDevice},
  ttnPhysicalDevice{ttnPhysicalDevice},
  ttnFramebuffer{ttnFramebuffer},
  ttnRenderpass{ttnRenderpass},
  ttnSwapChain{ttnSwapChain},
  ttnGraphicPipeline{ttnGraphicPipeline}
{
  VkCommandPoolCreateInfo commandPoolCreateInfo {};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = this->ttnPhysicalDevice.getQueueFamilyIndices().graphicsFamily.value();

  if (vkCreateCommandPool(this->ttnLogicalDevice.getDevice(), &commandPoolCreateInfo, nullptr, &this->commandPool) != VK_SUCCESS) {
    throw std::runtime_error("could not create command pool");
  }

  VkCommandBufferAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = this->commandPool;
  allocInfo.commandBufferCount = 1;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  if (vkAllocateCommandBuffers(this->ttnLogicalDevice.getDevice(), &allocInfo, &this->commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("could not create command buffer");
  }
}

Ttn::commands::Ttn_Command::~Ttn_Command() {
  vkFreeCommandBuffers(this->ttnLogicalDevice.getDevice(), this->commandPool, 1, &this->commandBuffer);
  vkDestroyCommandPool(this->ttnLogicalDevice.getDevice(), this->commandPool, nullptr);
}

void Ttn::commands::Ttn_Command::recordCommandBuffer(uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(this->commandBuffer, &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("could not start command buffer recording");
  }

  VkRenderPassBeginInfo renderPassBeginInfo {};
  renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassBeginInfo.renderPass = this->ttnRenderpass.getRenderpass();
  renderPassBeginInfo.framebuffer = this->ttnFramebuffer.getSwapChainFrameBuffers()[imageIndex];
  renderPassBeginInfo.renderArea.offset = {0, 0};
  renderPassBeginInfo.renderArea.extent = this->ttnSwapChain.getSwapChainExtent();

  VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
  renderPassBeginInfo.clearValueCount = 1;
  renderPassBeginInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(this->commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(this->commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->ttnGraphicPipeline.getPipeline());
  
  VkViewport viewport {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(this->ttnSwapChain.getSwapChainExtent().width);
  viewport.height = static_cast<float>(this->ttnSwapChain.getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(this->commandBuffer, 0, 1, &viewport);

  VkRect2D scissor {};
  scissor.offset = {0, 0};
  scissor.extent = this->ttnSwapChain.getSwapChainExtent();
  vkCmdSetScissor(this->commandBuffer, 0, 1, &scissor);
  vkCmdDraw(this->commandBuffer, 3, 1, 0, 0);

  vkCmdEndRenderPass(this->commandBuffer);

  if (vkEndCommandBuffer(this->commandBuffer) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer");
  }
}