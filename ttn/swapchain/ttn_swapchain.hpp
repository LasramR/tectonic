#pragma once

#include <vector>

#include <ttn/devices/ttn_physical_device.hpp>
#include <ttn/graphics/ttn_window.hpp>
#include "vulkan/vulkan.h"

namespace Ttn {

  namespace  swapchain {

    class Ttn_SwapChain {

      private:
      VkDevice vkDevice;
      const Ttn::devices::SwapChainSupportDetails& swapChainSupportDetails;
      const Ttn::devices::QueueFamilyIndices& queueFamilyIndices;
      Ttn::Ttn_Window* ttnWindow;
      
      VkSurfaceFormatKHR swapChainFormatMode;
      VkPresentModeKHR swapChainPresentMode;
      VkExtent2D swapExtent;
      uint32_t swapChainImageCount;

      VkSurfaceFormatKHR selectSurfaceFormatKHR(const std::vector<VkSurfaceFormatKHR>&);
      VkPresentModeKHR selectPresentModeKHR(const std::vector<VkPresentModeKHR>&);
      VkExtent2D selectSwapExtent(const VkSurfaceCapabilitiesKHR&);

      VkSwapchainCreateInfoKHR swapChainCreateInfo;
      VkSwapchainKHR swapChain;
      VkFormat swapChainFormat;
      std::vector<VkImage> imageBuffer;
      
      public:
        Ttn_SwapChain(VkDevice, const Ttn::devices::SwapChainSupportDetails&, const Ttn::devices::QueueFamilyIndices&, Ttn::Ttn_Window* ttnWindow);
        ~Ttn_SwapChain();

        std::vector<VkImage>* getImageBuffer();
        VkFormat getSwapChainFormat();
        VkExtent2D getSwapChainExtent();
    };

  };

};
