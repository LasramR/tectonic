#include "ttn_vulkan_app.hpp"

#include <stdexcept>
#include <format>
#include <GLFW/glfw3.h>

Ttn::VulkanApp::VulkanApp(std::string name, Ttn::Logger& logger) : vkApplicationInfo{}, vkInstanceCreateInfo{}, vkInstance{}, logger{logger} {
  this->vkApplicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  this->vkApplicationInfo.pApplicationName = name.c_str();
  this->vkApplicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  this->vkApplicationInfo.pEngineName = "Tectonic";
  this->vkApplicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  this->vkApplicationInfo.apiVersion = VK_API_VERSION_1_3;

  this->glfwExtensions = glfwGetRequiredInstanceExtensions(&this->glfwExtensionCount);

  this->vkInstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  this->vkInstanceCreateInfo.pApplicationInfo = &this->vkApplicationInfo;
  this->vkInstanceCreateInfo.enabledExtensionCount = this->glfwExtensionCount;
  this->vkInstanceCreateInfo.ppEnabledExtensionNames = this->glfwExtensions;
  this->vkInstanceCreateInfo.enabledLayerCount = 0;

  vkEnumerateInstanceExtensionProperties(nullptr, &this->vkExtensionCount, nullptr);
  this->vkExtensions.resize(this->vkExtensionCount);
  vkEnumerateInstanceExtensionProperties(nullptr, &this->vkExtensionCount, this->vkExtensions.data());

  if (vkCreateInstance(&this->vkInstanceCreateInfo, nullptr, &this->vkInstance) != VK_SUCCESS) {
    throw std::runtime_error("could not create vulkan instance");
  }
  
  this->logger.Info(std::format("required GLFW extensions: {}", this->glfwExtensionCount));

  for (int i; i < this->glfwExtensionCount; i++) {
    this->logger.Info(this->glfwExtensions[i]);
  }

  this->logger.Info(std::format("available Vulkan extensions: {}", this->vkExtensionCount));

  for (const auto& extension : this->vkExtensions) {
      this->logger.Info(extension.extensionName);
  }
}
Ttn::VulkanApp::~VulkanApp() {
  vkDestroyInstance(this->vkInstance, nullptr);
}

void Ttn::VulkanApp::initialize(Ttn::Ttn_WindowProperties windowProperties) {
  if (glfwInit() == GLFW_FALSE) {
    throw std::runtime_error("could not initialize Vulkan");
  }
  
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  this->window = new Ttn::Ttn_Window(this->vkApplicationInfo.pApplicationName, windowProperties);
}

void Ttn::VulkanApp::cleanUp() {
  delete this->window;
  glfwTerminate();
}

void Ttn::VulkanApp::run() {
  while(!this->window->ShouldClose()) {
      glfwPollEvents();
  }
}