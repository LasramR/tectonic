#include "ttn_physical_device.hpp"

#include <format>
#include <stdexcept>

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

  this->logger.Info(std::format("selected GPU {} ({})", this->vkPhysicalDeviceProperties.deviceID, this->vkPhysicalDeviceProperties.deviceName));


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

  return deviceScore;
}