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

Ttn::VulkanApp::VulkanApp(std::string name, Ttn::Ttn_WindowProperties windowProperties, const int MAX_FRAMES_IN_FLIGHT, const char* MODEL_PATH, const char* TEXTURE_PATH, Ttn::Logger& logger) : 
  vkApplicationInfo{}, 
  vkInstanceCreateInfo{}, 
  vkInstance{VK_NULL_HANDLE},
  MAX_FRAMES_IN_FLIGHT{MAX_FRAMES_IN_FLIGHT},
  MODEL_PATH {MODEL_PATH},
  TEXTURE_PATH {TEXTURE_PATH},
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

  this->logger.Info("Creating GLFW user pointer registry");
  this->glfwUserPointerRegistry = new Ttn::shared::GlfwUserPointerRegistry();
  glfwSetWindowUserPointer(this->window->getWindow(), (void*) this->glfwUserPointerRegistry);
  this->logger.Info("Binding window to glfw pointer registry");
  this->glfwUserPointerRegistry->ttnWindow = this->window;

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

  this->logger.Info(std::format("Loading model {}", this->MODEL_PATH));
  auto model = Ttn::vertex::TtnVertex(this->MODEL_PATH);

  this->logger.Info("Creating vertex buffer");
  this->ttnVertexBuffer = new Ttn::vertex::Ttn_Vertex_Buffer(this->ttnPhysicalDevice->GetVkPhysicalDevice(), this->ttnLogicalDevice->getDevice(), model, this->MAX_FRAMES_IN_FLIGHT);

  this->logger.Info("Creating command pool and command buffer");
  this->ttnCommand = new Ttn::commands::Ttn_Command(*this->ttnLogicalDevice, *this->ttnPhysicalDevice, *this->ttnSwapChain, this->MAX_FRAMES_IN_FLIGHT, *this->ttnVertexBuffer);

  this->logger.Info("Creating Multi aliasing");
  this->ttnMsaa = new Ttn::msaa::Ttn_Msaa(this->ttnLogicalDevice->getDevice(), *this->ttnPhysicalDevice, *this->ttnSwapChain);
  
  this->logger.Info("Creating render pass");
  this->ttnRenderpass = new Ttn::pipelines::Ttn_Renderpass(this->ttnLogicalDevice->getDevice(), this->ttnSwapChain->getSwapChainFormat(), *this->ttnPhysicalDevice, *this->ttnMsaa);

  this->logger.Info("Creating depth image");
  this->ttnDepth = new Ttn::depth::Ttn_Depth(this->ttnLogicalDevice->getDevice(), *this->ttnPhysicalDevice, *this->ttnSwapChain, *this->ttnVertexBuffer, *this->ttnCommand);
  
  this->logger.Info("Creating frame buffers");
  this->ttnFramebuffer = new Ttn::graphics::Ttn_Framebuffer(this->ttnLogicalDevice->getDevice(), *this->ttnSwapChain, *this->ttnImageView, *this->ttnRenderpass, this->ttnDepth->depthImageView, this->ttnMsaa->colorImageView);

  this->logger.Info(std::format("Loading textures {}", this->TEXTURE_PATH));
  this->ttnTexture = new Ttn::textures::Ttn_Texture(this->ttnLogicalDevice->getDevice(), this->ttnPhysicalDevice->GetVkPhysicalDevice(), this->TEXTURE_PATH, *this->ttnVertexBuffer, *this->ttnCommand);

  this->logger.Info("Creating graphic pipeline");
  this->ttnGraphicPipeline = new Ttn::pipelines::Ttn_Graphic_Pipeline(this->ttnLogicalDevice->getDevice(), this->logger, *this->ttnSwapChain, *this->ttnRenderpass, *this->ttnVertexBuffer, this->ttnTexture->textureImageView, this->ttnTexture->ttnSampler->textureSampler, this->MAX_FRAMES_IN_FLIGHT, model, this->ttnPhysicalDevice->msaaSamples);

  this->logger.Info("Binding graphic pipeline to command buffer");
  this->ttnCommand->bindGraphicPipeline(this->ttnGraphicPipeline);

  this->logger.Info("Binding frame buffer to command buffer");
  this->ttnCommand->bindFramebuffer(this->ttnFramebuffer);

  this->logger.Info("Binding frame render pass to command buffer");
  this->ttnCommand->bindRenderpass(this->ttnRenderpass);

  this->logger.Info("Creating sync objects");
  this->ttnSyncObjects.resize(this->MAX_FRAMES_IN_FLIGHT);
  for (int i = 0; i < this->MAX_FRAMES_IN_FLIGHT; i++) {
    this->ttnSyncObjects[i] = new Ttn::sync::Ttn_Sync_Objects(this->ttnLogicalDevice->getDevice());
  }

  this->logger.Info("Copying staging buffer to vertex buffer");
  auto stagingVertex = this->ttnVertexBuffer->createStagingBuffer(
    model.vertices.data(),
    this->ttnVertexBuffer->bufferSize
  );
  this->ttnCommand->copyBuffer(stagingVertex->vkBuffer, this->ttnVertexBuffer->vertexBuffer, this->ttnVertexBuffer->bufferSize);
  
  this->logger.Info("Copying staging buffer to index buffer");
  auto stagingIndex = this->ttnVertexBuffer->createStagingBuffer(
    model.indices.data(),
    this->ttnVertexBuffer->indexBufferSize
  );
  this->ttnCommand->copyBuffer(stagingIndex->vkBuffer, this->ttnVertexBuffer->indexBuffer, this->ttnVertexBuffer->indexBufferSize);

  vkDestroyBuffer(this->ttnLogicalDevice->getDevice(), stagingVertex->vkBuffer, nullptr);
  vkFreeMemory(this->ttnLogicalDevice->getDevice(), stagingVertex->vkDeviceMemory, nullptr);
  free(stagingVertex);

  vkDestroyBuffer(this->ttnLogicalDevice->getDevice(), stagingIndex->vkBuffer, nullptr);
  vkFreeMemory(this->ttnLogicalDevice->getDevice(), stagingIndex->vkDeviceMemory, nullptr);
  free(stagingIndex);

  this->logger.Info("Creating keyboard listener");
  this->keyboardInputListener = new Ttn::input::KeyboardInputListener(this->window->getWindow());
  this->logger.Info("binding keyboard listener to GlfwUserPointerRegistry");
  this->glfwUserPointerRegistry->keyboardInputListener = this->keyboardInputListener;

  this->logger.Info("Creating mouse listener");
  this->mouseStateListener = new Ttn::input::MouseStateListener(this->window->getWindow(), Ttn::input::MouseConstraint::ALWAYS_CENTERED | Ttn::input::MouseConstraint::HIDDEN_CURSOR);
  this->logger.Info("binding mouse listener to GlfwUserPointerRegistry");
  this->glfwUserPointerRegistry->mouseStateListener = this->mouseStateListener;

  this->logger.Info("Creating default camera");
  this->camera = new Ttn::camera::Camera(
    this->window->getWindow(),
    this->ttnSwapChain->getSwapChainExtent(),
    {4.0f, 4.0f, 4.0f},
    Ttn::camera::DefaultCameraOpts()
  );
  this->logger.Info("Creating rotate model animation");
  this->rotateModelAnimation = new Ttn::animations::RotateModel(*this->ttnVertexBuffer, *this->camera);
}

Ttn::VulkanApp::~VulkanApp() {
  vkDestroyInstance(this->vkInstance, nullptr);
}

void Ttn::VulkanApp::initialize() {}

void Ttn::VulkanApp::cleanUp() {
  delete this->rotateModelAnimation;
  delete this->mouseStateListener;
  delete this->keyboardInputListener;
  for (const auto& syncObject : this->ttnSyncObjects) {
    delete syncObject;
  }
  delete this->ttnGraphicPipeline;
  delete this->ttnTexture;
  delete this->ttnFramebuffer;
  delete this->ttnDepth;
  delete this->ttnMsaa;
  delete this->ttnCommand;
  delete this->ttnVertexBuffer;
  delete this->ttnRenderpass;
  delete this->ttnImageView;
  delete this->ttnSwapChain;
  delete this->ttnLogicalDevice;
  delete this->ttnPhysicalDevice;
  
  if (this->vkDebugger != nullptr) {
    delete this->vkDebugger;
  }
  
  delete this->window;
  delete this->glfwUserPointerRegistry;
  glfwTerminate();
}

void Ttn::VulkanApp::run() {
  bool firstFrame = true;
  while(!this->window->ShouldClose()) {
      glfwPollEvents();
      
      if (firstFrame) {
        firstFrame = false;
        this->mouseStateListener->ignoreMouseMove();
      }

      auto keyboardInput = this->keyboardInputListener->consumeCurrentKeyboardInput();
      if (keyboardInput.has_value()) {
        auto keyEvent = keyboardInput.value();
        auto key = keyEvent.key;

        if (keyEvent.isPressed) {

          if (!keyEvent.isRepeted) {
            if (key == GLFW_KEY_ESCAPE) {
              glfwSetWindowShouldClose(this->window->getWindow(), GLFW_TRUE);
            } else if (key == GLFW_KEY_R) {
              this->rotateModelAnimation->resetAnimation();
              this->camera->resetCamera();
            } else if (key == GLFW_KEY_F) {
              this->rotateModelAnimation->toggleAnimation();
            } else if (key == GLFW_KEY_F11) {
              this->window->toggleFullscreen();
              // DISABLE CAMERA HERE
            }
          }

          if (key == GLFW_KEY_W) {
            this->camera->moveWorldPositionRelativeToCameraAngle({-1.0f, 0.0f, 0.0f});
          } else if (key == GLFW_KEY_S) {
            this->camera->moveWorldPositionRelativeToCameraAngle({1.0f, 0.0f, 0.0f});
          } else if (key == GLFW_KEY_A) {
            this->camera->moveWorldHorizontalPositionRelativeToCameraAngle({0.0f, 1.0f, 0.0f});
          } else if (key == GLFW_KEY_D) {
            this->camera->moveWorldHorizontalPositionRelativeToCameraAngle({0.0f, -1.0f, 0.0f});
          } else if (key == GLFW_KEY_LEFT_SHIFT) {
            this->camera->moveAbsoluteWorldPosition({0.0f, 0.0f, -1.0f});
          } else if (key == GLFW_KEY_SPACE) {
            this->camera->moveAbsoluteWorldPosition({0.0f, 0.0f, 1.0f});
          } else if (key == GLFW_KEY_LEFT) {
            this->rotateModelAnimation->decreaseAcceleration();
          } else if (key == GLFW_KEY_RIGHT) {
            this->rotateModelAnimation->increaseAcceleration();
          }
        }
      }

      auto mouseInput = this->mouseStateListener->consumeMouseState();
      if ((mouseInput.leftBtn.isClicked || !this->camera->requireClick()) && mouseInput.hasMoved()) {
        this->camera->moveViewAngle(mouseInput.getMoveDelta());
      }      
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

  this->rotateModelAnimation->updateUniformBuffer(this->ttnSwapChain->getSwapChainExtent(), this->currentFrame);

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

  delete this->ttnFramebuffer;
  delete this->ttnDepth;
  delete this->ttnMsaa;
  delete this->ttnCommand;
  delete this->ttnImageView;
  delete this->ttnSwapChain;
  this->ttnSwapChain = new Ttn::swapchain::Ttn_SwapChain(
    this->ttnLogicalDevice->getDevice(),
    this->ttnPhysicalDevice->getSwapChainSupportDetails(),
    this->ttnPhysicalDevice->getQueueFamilyIndices(),
    this->window
  );
  this->ttnImageView = new Ttn::swapchain::Ttn_Image_View(this->ttnLogicalDevice->getDevice(), this->ttnSwapChain);
  this->ttnCommand = new Ttn::commands::Ttn_Command(*this->ttnLogicalDevice, *this->ttnPhysicalDevice, *this->ttnSwapChain, this->MAX_FRAMES_IN_FLIGHT, *this->ttnVertexBuffer);
  this->ttnMsaa = new Ttn::msaa::Ttn_Msaa(this->ttnLogicalDevice->getDevice(), *this->ttnPhysicalDevice, *this->ttnSwapChain);
  this->ttnDepth = new Ttn::depth::Ttn_Depth(this->ttnLogicalDevice->getDevice(), *this->ttnPhysicalDevice, *this->ttnSwapChain, *this->ttnVertexBuffer, *this->ttnCommand);
  this->ttnFramebuffer = new Ttn::graphics::Ttn_Framebuffer(this->ttnLogicalDevice->getDevice(), *this->ttnSwapChain, *this->ttnImageView, *this->ttnRenderpass, this->ttnDepth->depthImageView, this->ttnMsaa->colorImageView);
  this->ttnCommand->bindGraphicPipeline(this->ttnGraphicPipeline);
  this->ttnCommand->bindFramebuffer(this->ttnFramebuffer);
  this->ttnCommand->bindRenderpass(this->ttnRenderpass);
  this->camera->updateProjection(this->ttnSwapChain->getSwapChainExtent());
}