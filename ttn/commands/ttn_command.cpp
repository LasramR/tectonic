#include "ttn_command.hpp"

#include <stdexcept>

Ttn::commands::Ttn_Command::Ttn_Command(Ttn::devices::Ttn_Logical_Device& ttnLogicalDevice, Ttn::devices::Ttn_Physical_Device& ttnPhysicalDevice, Ttn::graphics::Ttn_Framebuffer& ttnFramebuffer, Ttn::pipelines::Ttn_Renderpass& ttnRenderpass, Ttn::swapchain::Ttn_SwapChain& ttnSwapChain, Ttn::pipelines::Ttn_Graphic_Pipeline& ttnGraphicPipeline, const int commandBuffersCount) :
  ttnLogicalDevice{ttnLogicalDevice},
  ttnPhysicalDevice{ttnPhysicalDevice},
  ttnFramebuffer{ttnFramebuffer},
  ttnRenderpass{ttnRenderpass},
  ttnSwapChain{ttnSwapChain},
  ttnGraphicPipeline{ttnGraphicPipeline},
  commandBuffersCount{commandBuffersCount}
{
  VkCommandPoolCreateInfo commandPoolCreateInfo {};
  commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  commandPoolCreateInfo.queueFamilyIndex = this->ttnPhysicalDevice.getQueueFamilyIndices().graphicsFamily.value();

  if (vkCreateCommandPool(this->ttnLogicalDevice.getDevice(), &commandPoolCreateInfo, nullptr, &this->commandPool) != VK_SUCCESS) {
    throw std::runtime_error("could not create command pool");
  }

  commandBuffers.resize(this->commandBuffersCount);

  VkCommandBufferAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = this->commandPool;
  allocInfo.commandBufferCount = this->commandBuffersCount;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

  if (vkAllocateCommandBuffers(this->ttnLogicalDevice.getDevice(), &allocInfo, this->commandBuffers.data()) != VK_SUCCESS) {
    throw std::runtime_error("could not create command buffer");
  }
}

Ttn::commands::Ttn_Command::~Ttn_Command() {
  vkFreeCommandBuffers(this->ttnLogicalDevice.getDevice(), this->commandPool, this->commandBuffersCount, this->commandBuffers.data());
  vkDestroyCommandPool(this->ttnLogicalDevice.getDevice(), this->commandPool, nullptr);
}

void Ttn::commands::Ttn_Command::recordCommandBuffer(uint32_t commandBufferIdx, uint32_t imageIndex) {
  VkCommandBufferBeginInfo beginInfo {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;
  beginInfo.pInheritanceInfo = nullptr;

  if (vkBeginCommandBuffer(this->commandBuffers[commandBufferIdx], &beginInfo) != VK_SUCCESS) {
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

  vkCmdBeginRenderPass(this->commandBuffers[commandBufferIdx], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
  vkCmdBindPipeline(this->commandBuffers[commandBufferIdx], VK_PIPELINE_BIND_POINT_GRAPHICS, this->ttnGraphicPipeline.getPipeline());
  
  VkViewport viewport {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(this->ttnSwapChain.getSwapChainExtent().width);
  viewport.height = static_cast<float>(this->ttnSwapChain.getSwapChainExtent().height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(this->commandBuffers[commandBufferIdx], 0, 1, &viewport);

  VkRect2D scissor {};
  scissor.offset = {0, 0};
  scissor.extent = this->ttnSwapChain.getSwapChainExtent();
  vkCmdSetScissor(this->commandBuffers[commandBufferIdx], 0, 1, &scissor);
  vkCmdDraw(this->commandBuffers[commandBufferIdx], 3, 1, 0, 0);

  vkCmdEndRenderPass(this->commandBuffers[commandBufferIdx]);

  if (vkEndCommandBuffer(this->commandBuffers[commandBufferIdx]) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer");
  }
}

void Ttn::commands::Ttn_Command::resetCommandBuffer(uint32_t commandBufferIdx) {
  vkResetCommandBuffer(this->commandBuffers[commandBufferIdx], 0);
}

VkResult Ttn::commands::Ttn_Command::submitCommandBuffer(uint32_t commandBufferIdx, uint32_t imageIndex, VkSemaphore waitSemaphore, VkSemaphore finishedSemaphore, VkFence safeBufferUseFence) {
  VkSubmitInfo submitInfo {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &waitSemaphore;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &this->commandBuffers[commandBufferIdx];
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &finishedSemaphore;

  if (vkQueueSubmit(this->ttnLogicalDevice.getGraphicsQueue(), 1, &submitInfo, safeBufferUseFence) != VK_SUCCESS) {
    throw std::runtime_error("failed submitting draw command to command buffer");
  }

  VkPresentInfoKHR presentInfo {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &finishedSemaphore;

  VkSwapchainKHR swapChains[] = { this->ttnSwapChain.getSwapChain() };
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = swapChains;
  presentInfo.pImageIndices = &imageIndex;
  presentInfo.pResults = nullptr;

  return vkQueuePresentKHR(this->ttnLogicalDevice.getPresentQueue(), &presentInfo);
}