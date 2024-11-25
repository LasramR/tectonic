#include "ttn_physical_device.hpp"

#include <format>
#include <set>
#include <stdexcept>

bool Ttn::devices::QueueFamilyIndices::isComplete() {
  return this->graphicsFamily.has_value() && this->presentFamily.has_value();
}

const std::vector<const char*> Ttn::devices::Ttn_Physical_Device::vkRequiredDeviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

Ttn::devices::Ttn_Physical_Device::Ttn_Physical_Device(VkInstance vkInstance, VkSurfaceKHR vkSurface, Ttn::Logger& logger) : 
  vkInstance{vkInstance},
  vkSurface{vkSurface},
  logger{logger}, 
  vkPhysicalDevice{VK_NULL_HANDLE}, 
  physicalDeviceScore{0} 
{
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

  this->swapChainSupportDetails = this->querySwapChainSupportDetails(this->vkPhysicalDevice);

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

  if (!this->findQueueFamily(vkPhysicalDevice, vkPhysicalDeviceProperties).isComplete()) {
    return 0;
  }

  if (!this->CheckDeviceExtensionSupport(vkPhysicalDevice)) {
    return 0;
  }

  SwapChainSupportDetails swapChainSupportDetails = this->querySwapChainSupportDetails(vkPhysicalDevice);
  if (swapChainSupportDetails.formats.empty() || swapChainSupportDetails.presentModes.empty()) {
    return 0;
  }

  VkPhysicalDeviceFeatures supportedFeatures;
  vkGetPhysicalDeviceFeatures(vkPhysicalDevice, &supportedFeatures);
  if (!supportedFeatures.samplerAnisotropy) {
    return 0;
  }

  return deviceScore;
}

bool Ttn::devices::Ttn_Physical_Device::CheckDeviceExtensionSupport(VkPhysicalDevice vkPhysicalDevice) {
  uint32_t deviceExtensionsCount;
  vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &deviceExtensionsCount, nullptr);
  std::vector<VkExtensionProperties> availableDeviceExtensionsCount(deviceExtensionsCount);
  vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &deviceExtensionsCount, availableDeviceExtensionsCount.data());

  std::set<std::string> requiredExtensions(Ttn::devices::Ttn_Physical_Device::vkRequiredDeviceExtensions.begin(), Ttn::devices::Ttn_Physical_Device::vkRequiredDeviceExtensions.end());
  for (const auto& extension : availableDeviceExtensionsCount) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

Ttn::devices::QueueFamilyIndices Ttn::devices::Ttn_Physical_Device::findQueueFamily(VkPhysicalDevice vkPhysicalDevice, VkPhysicalDeviceProperties vkPhysicalDeviceProperties) {
  uint32_t queueFamilyCount;

  vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount); 
  vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());
  
  int i = 0;
  for (const auto& queueFamilyProperty : queueFamilyProperties) {
    QueueFamilyIndices queueFamilyIndices {};
  
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(vkPhysicalDevice, i, this->vkSurface, &presentSupport);

    if (presentSupport) {
      queueFamilyIndices.presentFamily = i;
    }

    if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndices.graphicsFamily = i;
    }

    if (queueFamilyIndices.isComplete()) {
      return queueFamilyIndices;
    }
    i++;
  }

  return {};
}

Ttn::devices::SwapChainSupportDetails Ttn::devices::Ttn_Physical_Device::querySwapChainSupportDetails(VkPhysicalDevice vkPhysicalDevice) {
  Ttn::devices::SwapChainSupportDetails swapChainSupportDetails {};

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, this->vkSurface, &swapChainSupportDetails.capabilities);

  uint32_t formatsCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, this->vkSurface, &formatsCount, nullptr);

  if (formatsCount != 0) {
    swapChainSupportDetails.formats.resize(formatsCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkPhysicalDevice, this->vkSurface, &formatsCount, swapChainSupportDetails.formats.data());
  }

  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, this->vkSurface, &presentModeCount, nullptr);

  if (presentModeCount != 0) {
    swapChainSupportDetails.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(vkPhysicalDevice, this->vkSurface, &presentModeCount, swapChainSupportDetails.presentModes.data());
  }

  return swapChainSupportDetails;
}

Ttn::devices::QueueFamilyIndices Ttn::devices::Ttn_Physical_Device::getQueueFamilyIndices() {
  return this->queueFamily;
}
VkPhysicalDevice Ttn::devices::Ttn_Physical_Device::GetVkPhysicalDevice() {
  return this->vkPhysicalDevice;
}

Ttn::devices::SwapChainSupportDetails Ttn::devices::Ttn_Physical_Device::getSwapChainSupportDetails() {
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkPhysicalDevice, this->vkSurface, &swapChainSupportDetails.capabilities);
  return this->swapChainSupportDetails;
}

const std::vector<const char*>& Ttn::devices::Ttn_Physical_Device::getRequiredDeviceExtension() {
  return Ttn::devices::Ttn_Physical_Device::vkRequiredDeviceExtensions;
}
