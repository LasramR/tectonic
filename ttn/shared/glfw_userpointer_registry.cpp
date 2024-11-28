#include "glfw_userpointer_registry.hpp"

Ttn::shared::GlfwUserPointerRegistry::GlfwUserPointerRegistry() : 
  ttnWindow {nullptr},
  keyboardInputListener {nullptr},
  mouseStateListener {nullptr}
{}

Ttn::shared::GlfwUserPointerRegistry::~GlfwUserPointerRegistry() {}
