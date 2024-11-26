#include "glfw_userpointer_registry.hpp"

Ttn::shared::GlfwUserPointerRegistry::GlfwUserPointerRegistry() : 
  ttnWindow {nullptr},
  keyboardInputListener {nullptr}
{}

Ttn::shared::GlfwUserPointerRegistry::~GlfwUserPointerRegistry() {}
