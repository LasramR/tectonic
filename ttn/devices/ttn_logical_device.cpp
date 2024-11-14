#include "ttn_logical_device.hpp"

Ttn::devices::Ttn_Logical_Device::Ttn_Logical_Device(VkInstance vkInstance, Ttn::devices::Ttn_Physical_Device* ttnPhysicalDevice, Ttn::Logger* logger, const std::vector<const char*>& validationLayers) : 
  vkInstance{vkInstance}, 
  ttnPhysicalDevice{ttnPhysicalDevice},
  logger{logger},
  validationLayers{validationLayers}
{
  this->queueCreateInfo = {};
  this->queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  this->queueCreateInfo.queueFamilyIndex = this->ttnPhysicalDevice->getQueueFamilyIndices().graphicsFamily.value();
  this->queueCreateInfo.queueCount = 1;
  this->queueCreateInfo.pQueuePriorities = &this->queuePriority;

  VkPhysicalDeviceFeatures deviceFeatures{}; // empty for now

  this->vkDeviceCreateInfo = {};
  this->vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  this->vkDeviceCreateInfo.queueCreateInfoCount = 1;
  this->vkDeviceCreateInfo.pQueueCreateInfos = &this->queueCreateInfo;
  this->vkDeviceCreateInfo.pEnabledFeatures = &deviceFeatures;

  if (0 < this->validationLayers.size()) {
    this->vkDeviceCreateInfo.enabledLayerCount = 0;
  } else {
    this->vkDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(this->validationLayers.size());
    this->vkDeviceCreateInfo.ppEnabledLayerNames = this->validationLayers.data();
  }

  if (vkCreateDevice(this->ttnPhysicalDevice->GetVkPhysicalDevice(), &this->vkDeviceCreateInfo, nullptr, &this->vkDevice) != VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device");
  }
  this->logger->Info("logical device created");

  vkGetDeviceQueue(this->vkDevice, this->ttnPhysicalDevice->getQueueFamilyIndices().graphicsFamily.value(), 0, &this->queue);
  this->logger->Info("queue created");
}

Ttn::devices::Ttn_Logical_Device::~Ttn_Logical_Device() {
  vkDestroyDevice(this->vkDevice, nullptr);
}