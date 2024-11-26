#pragma once

#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <ttn/debug/ttn_vulkan_debugger.hpp>
#include <ttn/devices/ttn_physical_device.hpp>
#include <ttn/devices/ttn_logical_device.hpp>
#include <ttn/graphics/ttn_window.hpp>
#include <ttn/shared/logger.hpp>
#include <ttn/swapchain/ttn_swapchain.hpp>
#include <ttn/swapchain/ttn_image_view.hpp>
#include <ttn/pipelines/ttn_graphic_pipeline.hpp>
#include <ttn/pipelines/ttn_renderpass.hpp>
#include <ttn/graphics/ttn_framebuffer.hpp>
#include <ttn/commands/ttn_command.hpp>
#include <ttn/sync/ttn_sync_objects.hpp>
#include <ttn/vertex/ttn_vertex_buffer.hpp>
#include <ttn/pipelines/ttn_uniform_object_buffer.hpp>
#include <ttn/textures/ttn_textures.hpp>
#include <ttn/depth/ttn_depth.hpp>
#include <ttn/msaa/ttn_msaa.hpp>
#include <ttn/input/keyboard_input.hpp>
#include <ttn/shared/glfw_userpointer_registry.hpp>
#include <ttn/animations/rotate_model.hpp>

#include <string>
#include <vector>

#include "vulkan/vulkan.h"


namespace Ttn {

  class VulkanApp {

    private:
    VkInstance vkInstance;
    VkApplicationInfo vkApplicationInfo;
    VkInstanceCreateInfo vkInstanceCreateInfo;

    uint32_t glfwExtensionCount;
    std::vector<const char*> glfwExtensions;

    uint32_t vkExtensionCount;
    std::vector<VkExtensionProperties> vkExtensions; 
    static const std::vector<const char*> vkRequiredExtensions;

    uint32_t enabledExtensionsCount;
    std::vector<const char*> enabledExtensions;

    static const std::vector<const char*> vkValidationLayers;

    #ifdef NDEBUG
      static constexpr bool vkEnableValidationLayers = false;
    #else
      static constexpr bool vkEnableValidationLayers = true;
    #endif

    Ttn_Window* window;
    Ttn::Logger& logger;
    
    uint32_t vkAvailableValidationLayerCount;
    std::vector<VkLayerProperties> vkAvailableValidationLayerProperties;
    bool checkValidationLayerSupport();

    Ttn::debug::Vulkan_Debugger* vkDebugger;
    Ttn::devices::Ttn_Physical_Device* ttnPhysicalDevice;
    Ttn::devices::Ttn_Logical_Device* ttnLogicalDevice;
    Ttn::swapchain::Ttn_SwapChain* ttnSwapChain;
    Ttn::swapchain::Ttn_Image_View* ttnImageView;
    Ttn::pipelines::Ttn_Graphic_Pipeline* ttnGraphicPipeline;
    Ttn::pipelines::Ttn_Renderpass* ttnRenderpass;
    Ttn::graphics::Ttn_Framebuffer* ttnFramebuffer;
    Ttn::commands::Ttn_Command* ttnCommand;
    std::vector<Ttn::sync::Ttn_Sync_Objects*> ttnSyncObjects;

    const int MAX_FRAMES_IN_FLIGHT;
    uint32_t currentFrame;
    bool frameBufferResized = false;

    Ttn::vertex::Ttn_Vertex_Buffer* ttnVertexBuffer;
    Ttn::textures::Ttn_Texture* ttnTexture;
    Ttn::depth::Ttn_Depth* ttnDepth;
    
    const char* MODEL_PATH;
    const char* TEXTURE_PATH;

    Ttn::msaa::Ttn_Msaa* ttnMsaa;

    Ttn::shared::GlfwUserPointerRegistry* glfwUserPointerRegistry;
    Ttn::input::KeyboardInputListener* keyboardInputListener;

    Ttn::animations::RotateModel* rotateModelAnimation;


    public:
      VulkanApp(std::string name, Ttn::Ttn_WindowProperties windowProperties, const int MAX_FRAMES_IN_FLIGHT, const char*, const char*, Ttn::Logger& logger);
      ~VulkanApp();
      void initialize();
      void run();
      void cleanUp();
      void drawFrame();
      void recreateSwapChain();
  };

}