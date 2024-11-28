#include "mouse_state.hpp"

#include <ttn/shared/glfw_userpointer_registry.hpp>

bool Ttn::input::MouseState::hasMoved() {
  return this->currentMousePos.xpos != this->previousMousePos.xpos
    || this->currentMousePos.ypos != this->previousMousePos.ypos;
}

glm::vec2 Ttn::input::MouseState::getMoveDelta() {
  return { this->currentMousePos.xpos - this->previousMousePos.xpos, this->currentMousePos.ypos - this->previousMousePos.ypos };
}

Ttn::input::MouseConstraint Ttn::input::DefaultMouseConstraint() {
  return Ttn::input::MouseConstraint::NO_CONSTRAINT;
}

Ttn::input::MouseStateListener::MouseStateListener(GLFWwindow* window, MouseConstraintFlags constraints) :
  window {window},
  constraints {constraints},
  state {}
{
  glfwSetMouseButtonCallback(this->window, this->mouseBtnCallback);
  glfwSetCursorPosCallback(this->window, this->mouseMoveCallback);
  if (this->constraints & Ttn::input::MouseConstraint::ALWAYS_CENTERED) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(this->window, &windowWidth, &windowHeight);
    glfwSetCursorPos(this->window, windowWidth / 2, windowHeight / 2);
  }
  glfwGetCursorPos(this->window, &this->state.previousMousePos.xpos, &this->state.previousMousePos.ypos);

  if (this->constraints & Ttn::input::MouseConstraint::HIDDEN_CURSOR) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }
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

  if (mouseStateListener->constraints & Ttn::input::MouseConstraint::ALWAYS_CENTERED) {
    int windowWidth, windowHeight; // maybe with swap chain extent we can optimize this part to omit some glfw api calls;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    mouseStateListener->state.previousMousePos = {static_cast<double>(windowWidth / 2), static_cast<double>(windowHeight / 2)}; 
    glfwSetCursorPos(window, windowWidth / 2, windowHeight / 2);
  }
}

const Ttn::input::MouseState Ttn::input::MouseStateListener::consumeMouseState() {
  auto copy = this->state;

  this->state.previousMousePos = this->state.currentMousePos;
  
  return copy;
}

void Ttn::input::MouseStateListener::ignoreMouseMove() {
  this->state.previousMousePos = this->state.currentMousePos;
}