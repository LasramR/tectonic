#include "ttn_physical_device.hpp"

#include <format>
#include <stdexcept>

bool Ttn::devices::QueueFamilyIndices::isComplete() {
  return this->graphicsFamily.has_value();
}

Ttn::devices::Ttn_Physical_Device::Ttn_Physical_Device(VkInstance vkInstance, Ttn::Logger& logger) : vkInstance{vkInstance}, logger{logger}, vkPhysicalDevice{VK_NULL_HANDLE}, physicalDeviceScore{0} {
  vkEnumeratePhysicalDevices(this->vkInstance, &this->deviceCount, nullptr);

  if (this->deviceCount == 0) {
    throw std::runtime_error("no available GPU with Vulkan support");
  }

  this->logger.Info(std::format("{} GPU supported", this->deviceCount));

  this->availablePhysicalDevices.resize(this->deviceCount);
  vkEnumeratePhysicalDevices(this->vkInstance, &this->deviceCount, this->availablePhysicalDevices.data());

  for (const auto& device : this->availablePhysicalDevices) {
    VkPhysicalDeviceProperties deviceProperties {};
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    VkPhysicalDeviceFeatures deviceFeatures {};
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    uint32_t deviceScore = this->getPhysicalDeviceScore(device, deviceProperties, deviceFeatures);
    if (this->physicalDeviceScore < deviceScore) {
      this->vkPhysicalDevice = device;
      this->vkPhysicalDeviceProperties = deviceProperties;
      this->vkPhysicalDeviceFeatures = deviceFeatures;
      this->physicalDeviceScore = deviceScore;
    }
  }

  if (this->vkPhysicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("no GPU with Vulkan support");
  }

  this->queueFamily = this->findQueueFamily(this->vkPhysicalDevice, this->vkPhysicalDeviceProperties);

  if (!this->queueFamily.isComplete()) {
    throw std::runtime_error("no queueFamily supported with available GPU");
  }

  this->logger.Info(std::format("selected GPU {} ({}), queueFamily {}", this->vkPhysicalDeviceProperties.deviceID, this->vkPhysicalDeviceProperties.deviceName, this->queueFamily.graphicsFamily.value()));
}

Ttn::devices::Ttn_Physical_Device::~Ttn_Physical_Device() {}

uint32_t Ttn::devices::Ttn_Physical_Device::getPhysicalDeviceScore(VkPhysicalDevice vkPhysicalDevice, VkPhysicalDeviceProperties vkPhysicalDeviceProperties, VkPhysicalDeviceFeatures vkPhysicalDeviceFeatures) {
  uint32_t deviceScore = 0;
  
  if (vkPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    deviceScore += 1000;
  }
  
  deviceScore += vkPhysicalDeviceProperties.limits.maxImageDimension2D;
  
  if (!vkPhysicalDeviceFeatures.geometryShader) {
    return 0; // Application will not work without a geometry shader
  }

  if (! this->findQueueFamily(vkPhysicalDevice, vkPhysicalDeviceProperties).isComplete()) {
    return 0;
  }

  return deviceScore;
}

Ttn::devices::QueueFamilyIndices Ttn::devices::Ttn_Physical_Device::findQueueFamily(VkPhysicalDevice vkPhysicalDevice, VkPhysicalDeviceProperties vkPhysicalDeviceProperties) {
  QueueFamilyIndices queueFamilyIndices;
  uint32_t queueFamilyCount;

  vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount); 
  vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());
  
  int i = 0;
  for (const auto& queueFamilyProperty : queueFamilyProperties) {
    if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndices.graphicsFamily = i;
    }

    if (queueFamilyIndices.isComplete()) {
      break;
    }
    i++;
  }

  return queueFamilyIndices;
}

Ttn::devices::QueueFamilyIndices Ttn::devices::Ttn_Physical_Device::getQueueFamilyIndices() {
  return this->queueFamily;
}
VkPhysicalDevice Ttn::devices::Ttn_Physical_Device::GetVkPhysicalDevice() {
  return this->vkPhysicalDevice;
}