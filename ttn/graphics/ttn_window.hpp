#pragma once

#include <string>

// #define VK_USE_PLATFORM_XLIB_KHR // Tells Vulkan that we will be relying on XLib
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
// #define GLFW_EXPOSE_NATIVE_X11 // If not defined, related X11 will not be found
#include <GLFW/glfw3native.h>
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

      // VkXlibSurfaceCreateInfoKHR vkSurfaceCreateInfo;
      VkSurfaceKHR vkSurface;
      GLFWwindow* window;

    public:
    Ttn_Window(VkInstance, std::string, Ttn_WindowProperties);
    ~Ttn_Window();

    VkSurfaceKHR getSurface();
    GLFWwindow* getWindow();
    bool ShouldClose();
    void drawFrame();
  };

}