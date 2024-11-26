#include "keyboard_input.hpp"

#include <ttn/shared/glfw_userpointer_registry.hpp>

Ttn::input::KeyboardInputListener::KeyboardInputListener(GLFWwindow* window) : 
  window {window},
  currentKeyboardInput {}
{
  glfwSetKeyCallback(window, this->keyboardEventCallback);
}

Ttn::input::KeyboardInputListener::~KeyboardInputListener() {}

void Ttn::input::KeyboardInputListener::KeyboardInputListener::keyboardEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  auto keyboardInputListener = reinterpret_cast<Ttn::shared::GlfwUserPointerRegistry*>(glfwGetWindowUserPointer(window));
  Ttn::input::KeyboardInput input {};
  input.key = key;
  input.isPressed = action == GLFW_PRESS;
  keyboardInputListener->keyboardInputListener->currentKeyboardInput = input; // lol I can access private field here ?
}

std::optional<Ttn::input::KeyboardInput> Ttn::input::KeyboardInputListener::consumeCurrentKeyboardInput() {
  auto copy = this->currentKeyboardInput;
  this->currentKeyboardInput = {};
  return copy;
}