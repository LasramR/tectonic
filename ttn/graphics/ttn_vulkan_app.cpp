#include "ttn_vulkan_app.hpp"

#include <stdexcept>
#include <format>
#include<functional>
#include <map>
#include <cstring>
#include <GLFW/glfw3.h>

const std::vector<const char*> Ttn::VulkanApp::vkRequiredExtensions = {
    // "VK_KHR_xlib_surface",
};

const std::vector<const char*> Ttn::VulkanApp::vkValidationLayers = {
  "VK_LAYER_KHRONOS_validation"
};

Ttn::VulkanApp::VulkanApp(std::string name, Ttn::Ttn_WindowProperties windowProperties, const int MAX_FRAMES_IN_FLIGHT, Ttn::Logger& logger) : 
  vkApplicationInfo{}, 
  vkInstanceCreateInfo{}, 
  vkInstance{VK_NULL_HANDLE},
  MAX_FRAMES_IN_FLIGHT{MAX_FRAMES_IN_FLIGHT},
  currentFrame{0},
  logger{logger}
{
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
    this->logger.Info("Enabling Vulkan debugger");
    this->vkDebugger->createDebugMessenger(this->vkInstance);
  }

  this->logger.Info("Creating window and display surface");
  this->window = new Ttn::Ttn_Window(this->vkInstance, this->vkApplicationInfo.pApplicationName, windowProperties, this->logger);

  this->logger.Info("Selecting physical device");
  this->ttnPhysicalDevice = new Ttn::devices::Ttn_Physical_Device(this->vkInstance, this->window->getSurface(), this->logger);

  this->logger.Info("Creating logical device");
  this->ttnLogicalDevice = new Ttn::devices::Ttn_Logical_Device(this->vkInstance, this->ttnPhysicalDevice, &this->logger, this->vkValidationLayers);

  this->logger.Info("Creating swap chain");
  this->ttnSwapChain = new Ttn::swapchain::Ttn_SwapChain(
    this->ttnLogicalDevice->getDevice(),
    this->ttnPhysicalDevice->getSwapChainSupportDetails(),
    this->ttnPhysicalDevice->getQueueFamilyIndices(),
    this->window
  );
  
  this->logger.Info("Creating image views");
  this->ttnImageView = new Ttn::swapchain::Ttn_Image_View(this->ttnLogicalDevice->getDevice(), this->ttnSwapChain);

  this->logger.Info("Creating render pass");
  this->ttnRenderpass = new Ttn::pipelines::Ttn_Renderpass(this->ttnLogicalDevice->getDevice(), this->ttnSwapChain->getSwapChainFormat());

  this->logger.Info("Creating vertex buffer");
  this->ttnVertexBuffer = new Ttn::vertex::Ttn_Vertex_Buffer(this->ttnPhysicalDevice->GetVkPhysicalDevice(), this->ttnLogicalDevice->getDevice(), Ttn::vertex::TtnVertex::Default(), this->MAX_FRAMES_IN_FLIGHT);

  this->logger.Info("Creating graphic pipeline");
  this->ttnGraphicPipeline = new Ttn::pipelines::Ttn_Graphic_Pipeline(this->ttnLogicalDevice->getDevice(), this->logger, *this->ttnSwapChain, *this->ttnRenderpass, *this->ttnVertexBuffer, this->MAX_FRAMES_IN_FLIGHT);

  this->logger.Info("Creating frame buffers");
  this->ttnFramebuffer = new Ttn::graphics::Ttn_Framebuffer(this->ttnLogicalDevice->getDevice(), *this->ttnSwapChain, *this->ttnImageView, *this->ttnRenderpass);


  this->logger.Info("Creating command pool and command buffer");
  this->ttnCommand = new Ttn::commands::Ttn_Command(*this->ttnLogicalDevice, *this->ttnPhysicalDevice, *this->ttnFramebuffer, *this->ttnRenderpass, *this->ttnSwapChain, *this->ttnGraphicPipeline, this->MAX_FRAMES_IN_FLIGHT, *this->ttnVertexBuffer);

  this->logger.Info("Loading textures");
  this->ttnTexture = new Ttn::textures::Ttn_Texture(this->ttnLogicalDevice->getDevice(), this->ttnPhysicalDevice->GetVkPhysicalDevice(), "textures/texture.jpg", *this->ttnVertexBuffer, *this->ttnCommand);

  this->logger.Info("Creating sync objects");
  this->ttnSyncObjects.resize(this->MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < this->MAX_FRAMES_IN_FLIGHT; i++) {
    this->ttnSyncObjects[i] = new Ttn::sync::Ttn_Sync_Objects(this->ttnLogicalDevice->getDevice());
  }
}

Ttn::VulkanApp::~VulkanApp() {
  vkDestroyInstance(this->vkInstance, nullptr);
}

void Ttn::VulkanApp::initialize() {
  this->logger.Info("Copying staging buffer to vertex buffer");
  auto stagingVertex = this->ttnVertexBuffer->createStagingBuffer(
    this->ttnVertexBuffer->ttnVertex.vertices.data(),
    this->ttnVertexBuffer->bufferSize
  );
  this->ttnCommand->copyBuffer(stagingVertex->vkBuffer, this->ttnVertexBuffer->vertexBuffer, this->ttnVertexBuffer->bufferSize);
  
  auto stagingIndex = this->ttnVertexBuffer->createStagingBuffer(
    this->ttnVertexBuffer->ttnVertex.indices.data(),
    this->ttnVertexBuffer->indexBufferSize
  );
  this->ttnCommand->copyBuffer(stagingIndex->vkBuffer, this->ttnVertexBuffer->indexBuffer, this->ttnVertexBuffer->indexBufferSize);

  
  vkDestroyBuffer(this->ttnLogicalDevice->getDevice(), stagingVertex->vkBuffer, nullptr);
  vkFreeMemory(this->ttnLogicalDevice->getDevice(), stagingVertex->vkDeviceMemory, nullptr);
  free(stagingVertex);

  vkDestroyBuffer(this->ttnLogicalDevice->getDevice(), stagingIndex->vkBuffer, nullptr);
  vkFreeMemory(this->ttnLogicalDevice->getDevice(), stagingIndex->vkDeviceMemory, nullptr);
  free(stagingIndex);
}

void Ttn::VulkanApp::cleanUp() {
  for (const auto& syncObject : this->ttnSyncObjects) {
    delete syncObject;
  }
  delete this->ttnTexture;
  delete this->ttnCommand;
  delete this->ttnVertexBuffer;
  delete this->ttnFramebuffer;
  delete this->ttnGraphicPipeline;
  delete this->ttnRenderpass;
  delete this->ttnImageView;
  delete this->ttnSwapChain;
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
      this->drawFrame();
  }
  vkDeviceWaitIdle(this->ttnLogicalDevice->getDevice());
}

void Ttn::VulkanApp::drawFrame() {
  auto device = this->ttnLogicalDevice->getDevice();
  vkWaitForFences(device, 1, &this->ttnSyncObjects[this->currentFrame]->inFlightFence, VK_TRUE, UINT64_MAX);
  vkResetFences(device, 1, &this->ttnSyncObjects[this->currentFrame]->inFlightFence);

  bool recreateSwapChainBecauseSuboptimal = false;
  uint32_t nextImageIndex;
  VkResult acquireNextImageResult = vkAcquireNextImageKHR(device, this->ttnSwapChain->getSwapChain(), UINT64_MAX, this->ttnSyncObjects[this->currentFrame]->imageAvailableSemaphore, VK_NULL_HANDLE, &nextImageIndex);
  if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR) {
    this->recreateSwapChain();
    return;
  } else if (acquireNextImageResult == VK_SUBOPTIMAL_KHR) {
    recreateSwapChainBecauseSuboptimal = true;
  } else if (acquireNextImageResult != VK_SUCCESS) {
    throw std::runtime_error("failed to acquire swap chain image");
  }
  vkResetFences(device, 1, &this->ttnSyncObjects[currentFrame]->inFlightFence);

  this->ttnCommand->resetCommandBuffer(this->currentFrame);
  this->updateUniformBuffer(this->currentFrame);
  this->ttnCommand->recordCommandBuffer(this->currentFrame, nextImageIndex);
  
  VkResult presentQueueResult = this->ttnCommand->submitCommandBuffer(this->currentFrame, nextImageIndex, this->ttnSyncObjects[this->currentFrame]->imageAvailableSemaphore, this->ttnSyncObjects[this->currentFrame]->renderFinishedSemaphore, this->ttnSyncObjects[this->currentFrame]->inFlightFence);
  if (presentQueueResult == VK_ERROR_OUT_OF_DATE_KHR || presentQueueResult == VK_SUBOPTIMAL_KHR || this->frameBufferResized || recreateSwapChainBecauseSuboptimal) {
    this->frameBufferResized = false;
    this->recreateSwapChain();
  } else if (presentQueueResult != VK_SUCCESS) {
    throw std::runtime_error("failed to present swap chain image");
  }
  
  this->currentFrame = (this->currentFrame + 1) % this->MAX_FRAMES_IN_FLIGHT;
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

void Ttn::VulkanApp::updateUniformBuffer(uint32_t currentImage) {
  static auto startTime = std::chrono::high_resolution_clock::now();

  auto currentTime = std::chrono::high_resolution_clock::now();
  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
  
  // TBH I will need to check what the actual fuck is these following lines doing
  Ttn::pipelines::UniformBufferObject ubo {};
  ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.proj = glm::perspective(glm::radians(45.0f), this->ttnSwapChain->getSwapChainExtent().width / (float) this->ttnSwapChain->getSwapChainExtent().height, 0.1f, 10.0f);
  ubo.proj[1][1] *= -1;
  memcpy(this->ttnVertexBuffer->uniformBufferMapped[currentImage], &ubo, sizeof(ubo));
}

void Ttn::VulkanApp::recreateSwapChain() {
  // Minimized window handler
  // /!\ doesnt work on linux as minimized window keep their original width and height ()
  // Consider using glfwSetWindowIconifyCallback

  int width = 0, height = 0;
  glfwGetFramebufferSize(this->window->getWindow(), &width, &height);
  while (width == 0 || height == 0) {
    this->logger.Info("Window minimized, stop drawing");
    glfwGetFramebufferSize(this->window->getWindow(), &width, &height);
    glfwWaitEvents();
  }
  this->logger.Info("Recreating swap chain");
  vkDeviceWaitIdle(this->ttnLogicalDevice->getDevice());
  delete this->ttnCommand;
  delete this->ttnFramebuffer;
  delete this->ttnImageView;
  delete this->ttnSwapChain;
  this->ttnSwapChain = new Ttn::swapchain::Ttn_SwapChain(
    this->ttnLogicalDevice->getDevice(),
    this->ttnPhysicalDevice->getSwapChainSupportDetails(),
    this->ttnPhysicalDevice->getQueueFamilyIndices(),
    this->window
  );
  this->ttnImageView = new Ttn::swapchain::Ttn_Image_View(this->ttnLogicalDevice->getDevice(), this->ttnSwapChain);
  this->ttnFramebuffer = new Ttn::graphics::Ttn_Framebuffer(this->ttnLogicalDevice->getDevice(), *this->ttnSwapChain, *this->ttnImageView, *this->ttnRenderpass);
  this->ttnCommand = new Ttn::commands::Ttn_Command(*this->ttnLogicalDevice, *this->ttnPhysicalDevice, *this->ttnFramebuffer, *this->ttnRenderpass, *this->ttnSwapChain, *this->ttnGraphicPipeline, this->MAX_FRAMES_IN_FLIGHT, *this->ttnVertexBuffer);
}