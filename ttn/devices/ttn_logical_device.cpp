#include "ttn_logical_device.hpp"

#include <set>

Ttn::devices::Ttn_Logical_Device::Ttn_Logical_Device(VkInstance vkInstance, Ttn::devices::Ttn_Physical_Device* ttnPhysicalDevice, Ttn::Logger* logger, const std::vector<const char*>& validationLayers) : 
  vkInstance{vkInstance}, 
  ttnPhysicalDevice{ttnPhysicalDevice},
  logger{logger},
  validationLayers{validationLayers}
{
  std::set<uint32_t> uniqueQueueFamilies = {
    this->ttnPhysicalDevice->getQueueFamilyIndices().graphicsFamily.value(),
    this->ttnPhysicalDevice->getQueueFamilyIndices().presentFamily.value()
  };

  for (const uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &this->queuePriority;
    this->queuesCreateInfo.push_back(queueCreateInfo);
  }
  
  VkPhysicalDeviceFeatures deviceFeatures{}; // empty for now
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  this->vkDeviceCreateInfo = {};
  this->vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  this->vkDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(this->queuesCreateInfo.size());
  this->vkDeviceCreateInfo.pQueueCreateInfos = this->queuesCreateInfo.data();
  this->vkDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(this->ttnPhysicalDevice->getRequiredDeviceExtension().size());
  this->vkDeviceCreateInfo.ppEnabledExtensionNames = this->ttnPhysicalDevice->getRequiredDeviceExtension().data();
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

  vkGetDeviceQueue(this->vkDevice, this->ttnPhysicalDevice->getQueueFamilyIndices().graphicsFamily.value(), 0, &this->graphicsQueue);
  this->logger->Info("graphics queue created");

  vkGetDeviceQueue(this->vkDevice, this->ttnPhysicalDevice->getQueueFamilyIndices().presentFamily.value(), 0, &this->presentQueue);
  this->logger->Info("present queue created");
}

Ttn::devices::Ttn_Logical_Device::~Ttn_Logical_Device() {
  vkDestroyDevice(this->vkDevice, nullptr);
}

VkDevice Ttn::devices::Ttn_Logical_Device::getDevice() {
  return this->vkDevice;
}

VkQueue Ttn::devices::Ttn_Logical_Device::getGraphicsQueue() {
  return this->graphicsQueue;
}

VkQueue Ttn::devices::Ttn_Logical_Device::getPresentQueue() {
  return this->presentQueue;
}