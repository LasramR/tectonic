#include "ttn_window.hpp"

#include <string>
#include <stdexcept>

Ttn::Ttn_Window::Ttn_Window(VkInstance vkInstance, std::string name, Ttn_WindowProperties windowProperties) : 
  vkInstance{vkInstance},
  name{name}, 
  windowProperties{windowProperties}
{
  glfwWindowHint(GLFW_RESIZABLE, this->windowProperties.resizable ? GLFW_TRUE : GLFW_FALSE);
  this->window = glfwCreateWindow(this->windowProperties.width, this->windowProperties.height, this->name.c_str(), nullptr, nullptr);

  if (this->window == NULL) {
    throw std::runtime_error("could not open window");
  }

  //this->vkSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
  //this->vkSurfaceCreateInfo.dpy = glfwGetX11Display();
  //this->vkSurfaceCreateInfo.window = glfwGetX11Window(this->window);
  //this->vkSurfaceCreateInfo.flags = 0;
  //this->vkSurfaceCreateInfo.pNext = nullptr;

  //if (vkCreateXlibSurfaceKHR(this->vkInstance, &this->vkSurfaceCreateInfo, nullptr, &this->vkSurface) != VK_SUCCESS) {
  //  throw std::runtime_error("failed to create window surface");
  //}

  if (glfwCreateWindowSurface(this->vkInstance, this->window, nullptr, &this->vkSurface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface!");
  }
}
Ttn::Ttn_Window::~Ttn_Window() {
  vkDestroySurfaceKHR(this->vkInstance, this->vkSurface, nullptr);
  glfwDestroyWindow(this->window);
}

VkSurfaceKHR Ttn::Ttn_Window::getSurface() {
  return this->vkSurface;
}

GLFWwindow* Ttn::Ttn_Window::getWindow() {
  return this->window;
}

bool Ttn::Ttn_Window::ShouldClose() {
  return glfwWindowShouldClose(this->window);
}

void Ttn::Ttn_Window::drawFrame() {
  
}