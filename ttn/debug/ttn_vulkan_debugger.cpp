#include "ttn_vulkan_debugger.hpp"

#include <format>
#include <stdexcept>

Ttn::debug::Vulkan_Debugger::Vulkan_Debugger(VkInstanceCreateInfo* vkInstanceCreateInfo, Ttn::Logger& logger) : vkInstanceCreateInfo{vkInstanceCreateInfo}, logger{logger}, vkInstanceDebugUtilsMessengerCreateInfo{} {
  this->initializeDebugMessengerCreateInfo(&this->vkInstanceDebugUtilsMessengerCreateInfo);
  this->vkInstanceCreateInfo->pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &this->vkInstanceDebugUtilsMessengerCreateInfo;
}

Ttn::debug::Vulkan_Debugger::~Vulkan_Debugger() {
  for (const auto debugger : this->debugMessengers) {
    auto vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(debugger.vkInstance, "vkDestroyDebugUtilsMessengerEXT");
    if (vkDestroyDebugUtilsMessengerEXT != nullptr) {
      vkDestroyDebugUtilsMessengerEXT(debugger.vkInstance, *debugger.vkDebugUtilsMessengerEXT, nullptr);
      delete debugger.vkDebugUtilsMessengerEXT;
    }
  }
}

void Ttn::debug::Vulkan_Debugger::initializeDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT* createInfo) {
  createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  createInfo->pfnUserCallback = Ttn::debug::Vulkan_Debugger::vkDebuggerMessengerCallback;
  createInfo->pUserData = this;
}

VKAPI_ATTR VkBool32 VKAPI_CALL Ttn::debug::Vulkan_Debugger::vkDebuggerMessengerCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData
) {

  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    static_cast<Ttn::debug::Vulkan_Debugger*>(pUserData)->logger.Debug(
      Ttn::debug::VK_DEBUG_SEVERITY_MAP.at(messageSeverity),
      std::format("Validation layer: {}", pCallbackData->pMessage)
    );
  }
  
  return VK_FALSE;
};

VkDebugUtilsMessengerEXT* Ttn::debug::Vulkan_Debugger::createDebugMessenger(VkInstance vkInstance) {
  auto createMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(vkInstance, "vkCreateDebugUtilsMessengerEXT");
  if (createMessenger == nullptr) {
    throw std::runtime_error("could not load extension vkCreateDebugUtilsMessengerEXT");
  }
  
  VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo {};
  this->initializeDebugMessengerCreateInfo(&debugMessengerCreateInfo);
  
  auto debugMessenger = new VkDebugUtilsMessengerEXT{};
  if (createMessenger(vkInstance, &debugMessengerCreateInfo, nullptr, debugMessenger) != VK_SUCCESS) {
    throw new std::runtime_error("could not create debugger messenger");
  }
  
  this->debugMessengers.push_back({vkInstance, debugMessenger});

  return debugMessenger;
}