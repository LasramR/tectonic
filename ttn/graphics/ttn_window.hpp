#pragma once

#include <string>

#include <ttn/shared/logger.hpp>

#define GLFW_INCLUDE_VULKAN
// #define VK_USE_PLATFORM_XLIB_KHR // Tells Vulkan that we will be relying on XLib
#include <GLFW/glfw3.h>
// #define GLFW_EXPOSE_NATIVE_X11 // If not defined, related X11 will not be found
#include <GLFW/glfw3native.h> // Needed for WindowSurface creation;
#include "vulkan/vulkan.h"
// #include <vulkan/vulkan_xlib.h> // Include related XLib Vulkan types

namespace Ttn {

  struct Ttn_WindowProperties {
    int width;
    int height;
    bool resizable;
  };

  class Ttn_Window {
    private:
      VkInstance vkInstance;
      std::string name;
      Ttn_WindowProperties windowProperties;
      Ttn::Logger& logger;
      
      int actualWidth;
      int actualHeight;
      bool isFullscreen;

      // VkXlibSurfaceCreateInfoKHR vkSurfaceCreateInfo;
      VkSurfaceKHR vkSurface;
      GLFWwindow* window;
      bool resized;
      bool minimized;
    public:
    Ttn_Window(VkInstance, std::string, Ttn_WindowProperties, Ttn::Logger&);
    ~Ttn_Window();

    VkSurfaceKHR getSurface();
    GLFWwindow* getWindow();
    bool ShouldClose();
    bool hasResized();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    void toggleFullscreen();
  };

}