#pragma once

#include <map>
#include <string>

#include <ttn/shared/logger.hpp>

#include "vulkan/vulkan.h"

namespace Ttn {

  namespace debug {

    enum VkDebuggerMessageSeverity : uint16_t {
      ERROR = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      DIAGNOSTIC = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
      INFO = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT,
      WARNING = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
    };

    const std::unordered_map<uint32_t, std::string> VK_DEBUG_SEVERITY_MAP = { 
      {Ttn::debug::VkDebuggerMessageSeverity::DIAGNOSTIC ,"Diagnostic"}, 
      {Ttn::debug::VkDebuggerMessageSeverity::ERROR, "Error"}, 
      {Ttn::debug::VkDebuggerMessageSeverity::INFO, "Info"}, 
      {Ttn::debug::VkDebuggerMessageSeverity::WARNING, "Warning"}
    };

    typedef struct Debugger {
      VkInstance vkInstance;
      VkDebugUtilsMessengerEXT* vkDebugUtilsMessengerEXT;
    } Debugger;

    class Vulkan_Debugger {
      
      private:
        VkApplicationInfo& vkApplicationInfo;
        Ttn::Logger& logger;

        VkDebugUtilsMessengerCreateInfoEXT* vkInstanceDebugUtilsMessengerCreateInfo;
        void initializeDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo);
        static VKAPI_ATTR VkBool32 VKAPI_CALL vkDebuggerMessengerCallback(
          VkDebugUtilsMessageSeverityFlagBitsEXT,
          VkDebugUtilsMessageTypeFlagsEXT,
          const VkDebugUtilsMessengerCallbackDataEXT*,
          void*
        );
        std::vector<Debugger> debugMessengers;


      public:
        Vulkan_Debugger(VkApplicationInfo&, Ttn::Logger&);
        ~Vulkan_Debugger();
        VkDebugUtilsMessengerEXT* createDebuggerMessenger(VkInstance vkInstance);
    };
  };
}