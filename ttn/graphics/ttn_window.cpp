#include "ttn_window.hpp"

#include <string>
#include <stdexcept>

#include "GLFW/glfw3.h"

Ttn::Ttn_Window::Ttn_Window(std::string name, Ttn_WindowProperties windowProperties) : name{name}, windowProperties{windowProperties} {
  glfwWindowHint(GLFW_RESIZABLE, this->windowProperties.resizable ? GLFW_TRUE : GLFW_FALSE);
  this->window = glfwCreateWindow(this->windowProperties.width, this->windowProperties.height, this->name.c_str(), nullptr, nullptr);

  if (this->window == NULL) {
    throw std::runtime_error("could not open window");
  }
};

Ttn::Ttn_Window::~Ttn_Window() {
  glfwDestroyWindow(this->window);
}

bool Ttn::Ttn_Window::ShouldClose() {
  return glfwWindowShouldClose(this->window);
}
