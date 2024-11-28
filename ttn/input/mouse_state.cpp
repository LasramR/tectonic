#include "mouse_state.hpp"

#include <ttn/shared/glfw_userpointer_registry.hpp>

bool Ttn::input::MouseState::hasMoved() {
  return this->currentMousePos.xpos != this->previousMousePos.xpos
    || this->currentMousePos.ypos != this->previousMousePos.ypos;
}

glm::vec2 Ttn::input::MouseState::getMoveDelta() {
  return { this->currentMousePos.xpos - this->previousMousePos.xpos, this->currentMousePos.ypos - this->previousMousePos.ypos };
}


Ttn::input::MouseStateListener::MouseStateListener(GLFWwindow* window) :
  window {window},
  state {}
{
  glfwSetMouseButtonCallback(this->window, this->mouseBtnCallback);
  glfwSetCursorPosCallback(this->window, this->mouseMoveCallback);
  glfwGetCursorPos(this->window, &this->state.previousMousePos.xpos, &this->state.previousMousePos.ypos);
}

Ttn::input::MouseStateListener::~MouseStateListener() {
  glfwSetMouseButtonCallback(this->window, nullptr);
  glfwSetCursorPosCallback(this->window, nullptr);
}

#include <iostream>
#include <format>


void Ttn::input::MouseStateListener::mouseBtnCallback(GLFWwindow* window, int button, int action, int mods) {
  auto registry = reinterpret_cast<Ttn::shared::GlfwUserPointerRegistry*>(glfwGetWindowUserPointer(window));
  if (registry->mouseStateListener == nullptr) return;

  auto mouseStateListener = registry->mouseStateListener;
  
  Ttn::input::MouseBtn* mouseBtn;
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    mouseBtn = &mouseStateListener->state.leftBtn;
  } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
    mouseBtn = &mouseStateListener->state.rightBtn;
  }

  if (mouseBtn == nullptr) return;

  mouseBtn->isClicked = action == GLFW_PRESS;
  mouseBtn->isReleased = action == GLFW_RELEASE;
}

void Ttn::input::MouseStateListener::mouseMoveCallback(GLFWwindow* window, double xpos, double ypos) {
  auto registry = reinterpret_cast<Ttn::shared::GlfwUserPointerRegistry*>(glfwGetWindowUserPointer(window));
  if (registry->mouseStateListener == nullptr) return;

  auto mouseStateListener = registry->mouseStateListener;

  mouseStateListener->state.previousMousePos = mouseStateListener->state.currentMousePos; 
  mouseStateListener->state.currentMousePos = {xpos: xpos, ypos: ypos}; 

}

const Ttn::input::MouseState Ttn::input::MouseStateListener::consumeMouseState() {
  auto copy = this->state;

  this->state.previousMousePos = this->state.currentMousePos;
  
  return copy;
}

void Ttn::input::MouseStateListener::ignoreMouseMove() {
  this->state.previousMousePos = this->state.currentMousePos;
}