#include "ttn_sync_objects.hpp"

#include <stdexcept>

Ttn::sync::Ttn_Sync_Objects::Ttn_Sync_Objects(VkDevice vkDevice) : vkDevice{vkDevice} {
  VkSemaphoreCreateInfo semaphoreInfo{};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo{};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  if (vkCreateSemaphore(this->vkDevice, &semaphoreInfo, nullptr, &this->imageAvailableSemaphore) != VK_SUCCESS
    || vkCreateSemaphore(this->vkDevice, &semaphoreInfo, nullptr, &this->renderFinishedSemaphore) != VK_SUCCESS
    || vkCreateFence(this->vkDevice, &fenceInfo, nullptr, &this->inFlightFence) != VK_SUCCESS
  ) {
    throw std::runtime_error("failed to create sync objects");
  }
}

Ttn::sync::Ttn_Sync_Objects::~Ttn_Sync_Objects() {
  vkDestroySemaphore(this->vkDevice, this->imageAvailableSemaphore, nullptr);
  vkDestroySemaphore(this->vkDevice, this->renderFinishedSemaphore, nullptr);
  vkDestroyFence(this->vkDevice, this->inFlightFence, nullptr);
}