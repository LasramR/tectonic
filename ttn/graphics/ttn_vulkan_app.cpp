#include "ttn_vulkan_app.hpp"

#include <stdexcept>
#include <format>
#include<functional>
#include <map>
#include <cstring>
#include <GLFW/glfw3.h>

const std::vector<const char*> Ttn::VulkanApp::vkRequiredExtensions = {
    // "VK_KHR_xlib_surface"
};

const std::vector<const char*> Ttn::VulkanApp::vkValidationLayers = {
  "VK_LAYER_KHRONOS_validation"
};

Ttn::VulkanApp::VulkanApp(std::string name, Ttn::Ttn_WindowProperties windowProperties, Ttn::Logger& logger) : vkApplicationInfo{}, vkInstanceCreateInfo{}, vkInstance{} ,logger{logger} {
  this->logger.Info("Initializing GLFW with Vulkan API");
  if (glfwInit() == GLFW_FALSE) {
    throw std::runtime_error("could not initialize Vulkan");
  }
  
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  
  this->vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  this->vkApplicationInfo.pApplicationName = name.c_str();
  this->vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  this->vkApplicationInfo.pEngineName = "Tectonic";
  this->vkApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  this->vkApplicationInfo.apiVersion = VK_API_VERSION_1_3;

  auto glfwRawExtensions = glfwGetRequiredInstanceExtensions(&this->glfwExtensionCount);
  std::vector<const char*> glfwExtensions(glfwRawExtensions, glfwRawExtensions + this->glfwExtensionCount);
  this->glfwExtensions = glfwExtensions;

  this->vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  this->vkInstanceCreateInfo.pApplicationInfo = &this->vkApplicationInfo;

  if (this->vkEnableValidationLayers) {
    this->logger.Info("Enabling Vulkan validation layers");
    if (!this->checkValidationLayerSupport()) {
      throw std::runtime_error("validation layers requested but not available");
    }
    this->vkInstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(this->vkValidationLayers.size());
    this->vkInstanceCreateInfo.ppEnabledLayerNames = this->vkValidationLayers.data();
    this->glfwExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    this->glfwExtensionCount = static_cast<uint32_t>(this->glfwExtensions.size());
    this->logger.Info("loading vk debugger");
    this->vkDebugger = new Ttn::debug::Vulkan_Debugger(&this->vkInstanceCreateInfo, this->logger);
  } else {
    this->vkInstanceCreateInfo.enabledLayerCount = 0;
    this->vkInstanceCreateInfo.pNext = nullptr;
  }

  this->logger.Info(std::format("{} GLFW extensions required", this->glfwExtensionCount));

  for (int i; i < this->glfwExtensionCount; i++) {
    this->logger.Info(this->glfwExtensions[i]);
  }
  
  this->enabledExtensionsCount = this->glfwExtensionCount;
  std::vector<const char*> enabledExtensions(this->glfwExtensions.data(), this->glfwExtensions.data() + this->glfwExtensionCount);
  this->enabledExtensions = enabledExtensions;

  vkEnumerateInstanceExtensionProperties(nullptr, &this->vkExtensionCount, nullptr);
  this->vkExtensions.resize(this->vkExtensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &this->vkExtensionCount, this->vkExtensions.data());

  this->logger.Info(std::format("{} Vulkan extensions required", this->vkRequiredExtensions.size()));

  for (const auto& requiredExtension : this->vkRequiredExtensions) {
    bool isAvailable = false;
    for (const auto& availableExtension : this->vkExtensions) {
      if (strcmp(requiredExtension, availableExtension.extensionName) == 0) {
        isAvailable = true;
        break;
      } 
    }
    if (!isAvailable) {
      throw std::runtime_error(std::format("missing required extension {}", requiredExtension));
    }
    this->logger.Info(requiredExtension);
    this->enabledExtensionsCount += 1;
    this->enabledExtensions.push_back(requiredExtension);
  }

  this->vkInstanceCreateInfo.enabledExtensionCount = this->enabledExtensionsCount;
  this->vkInstanceCreateInfo.ppEnabledExtensionNames = this->enabledExtensions.data();


  this->logger.Info("Initializing Vulkan instance");
  if (vkCreateInstance(&this->vkInstanceCreateInfo, nullptr, &this->vkInstance) != VK_SUCCESS) {
    throw std::runtime_error("could not create vulkan instance");
  }

  if (this->vkEnableValidationLayers) {
    this->vkDebugger->createDebugMessenger(this->vkInstance);
  }

  this->logger.Info("Creating window and display surface");
  this->window = new Ttn::Ttn_Window(this->vkInstance, this->vkApplicationInfo.pApplicationName, windowProperties);

  this->logger.Info("Selecting physical device");
  this->ttnPhysicalDevice = new Ttn::devices::Ttn_Physical_Device(this->vkInstance, this->logger);

  this->logger.Info("Creating logical device");
  this->ttnLogicalDevice = new Ttn::devices::Ttn_Logical_Device(this->vkInstance, this->ttnPhysicalDevice, &this->logger, this->vkValidationLayers);

}

Ttn::VulkanApp::~VulkanApp() {
  vkDestroyInstance(this->vkInstance, nullptr);
}

void Ttn::VulkanApp::initialize() {}

void Ttn::VulkanApp::cleanUp() {
  delete this->ttnLogicalDevice;
  delete this->ttnPhysicalDevice;
  
  if (this->vkDebugger != nullptr) {
    delete this->vkDebugger;
  }
  
  delete this->window;
  glfwTerminate();
}

void Ttn::VulkanApp::run() {
  while(!this->window->ShouldClose()) {
      glfwPollEvents();
  }
}

bool Ttn::VulkanApp::checkValidationLayerSupport() {
  vkEnumerateInstanceLayerProperties(&this->vkAvailableValidationLayerCount, nullptr);
  
  this->vkAvailableValidationLayerProperties.resize(this->vkAvailableValidationLayerCount);
  vkEnumerateInstanceLayerProperties(&this->vkAvailableValidationLayerCount, this->vkAvailableValidationLayerProperties.data());

  for (const auto layerName : this->vkValidationLayers) {
    bool layerFound = false;

    for (const auto& layerProperty : this->vkAvailableValidationLayerProperties) {
      if (strcmp(layerName, layerProperty.layerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (!layerFound) {
      return false;
    }
  }

  return true;
}
