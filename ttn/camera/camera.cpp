#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

Ttn::camera::CameraOpts Ttn::camera::DefaultCameraOpts() {
  return {
    behaviour: Ttn::camera::CameraBehaviour::CAMERA_FOLLOW_POINTS_TO,
    mode: Ttn::camera::CameraMode::CAMERA_CURSOR_CENTER,
    sensitivity: 1.0f
  };
}

Ttn::camera::Camera::Camera(GLFWwindow* window, VkExtent2D clipArea, glm::vec3 worldPosition, glm::vec3 pointsTo, CameraOpts options) :
  window {window},
  clipArea {clipArea},
  worldPosition {worldPosition},
  pointsTo {pointsTo},
  upAngle {0.0f, 0.0f, 1.0f},
  options {options}
{
  glm::vec3 initialCameraDirection = glm::normalize(this->pointsTo - this->worldPosition);
  this->viewAngle = {
    glm::degrees(atan2(initialCameraDirection.y, initialCameraDirection.x)),
    glm::degrees(asin(initialCameraDirection.z))
  };
  this->computeViewMatrix();
  this->computeProjectionMatrix();

  this->initialViewAngle = this->viewAngle;
  this->initialWorldPosition = this->worldPosition;
  this->initialPointsTo = this->pointsTo;
}

Ttn::camera::Camera::Camera(GLFWwindow* window, VkExtent2D clipArea, glm::vec3 worldPosition, CameraOpts options) : 
Camera(
  window,
  clipArea,
  worldPosition,
  {0.0f, 0.0f, 1.0f},
  options
) {}

Ttn::camera::Camera::Camera(GLFWwindow* window, VkExtent2D clipArea, CameraOpts options) : 
Camera(
  window,
  clipArea,
  {2.0f, 2.0f, 2.0f},
  {0.0f, 0.0f, 1.0f},
  options
) {}


Ttn::camera::Camera::~Camera() {}

void Ttn::camera::Camera::computeViewMatrix() {
  this->viewMatrix = glm::lookAt(this->worldPosition, this->pointsTo, this->upAngle);
}

void Ttn::camera::Camera::computeProjectionMatrix() {
  // glm::perspective(FOV, aspect ratio, how close an object can be to the camera, how far can on object be to the camera)
  this->projMatrix = glm::perspective(glm::radians(45.0f), this->clipArea.width / (float) this->clipArea.height, 0.1f, 1000.0f);
  this->projMatrix[1][1] *= -1;
}


void Ttn::camera::Camera::moveWorldPosition(glm::vec3 newWorldPosition) {
  // TODO
}
void Ttn::camera::Camera::moveViewAngle(glm::vec2 moveDelta) {
  if (this->options.behaviour == Ttn::camera::CameraBehaviour::CAMERA_FOLLOW_POINTS_TO) {
    moveDelta = -moveDelta;
  }

  this->viewAngle.x += moveDelta.x * this->options.sensitivity;
  this->viewAngle.y = glm::clamp(this->viewAngle.y + moveDelta.y * this->options.sensitivity, -89.0f, 89.0f);

  glm::vec3 cameraMove {
    cos(glm::radians(this->viewAngle.y)) * cos(glm::radians(this->viewAngle.x)),
    cos(glm::radians(this->viewAngle.y)) * sin(glm::radians(this->viewAngle.x)),
    sin(glm::radians(this->viewAngle.y))
  };

  this->pointsTo = this->worldPosition + cameraMove;

  this->computeViewMatrix();
}

const glm::mat4& Ttn::camera::Camera::getViewSpace() {
  return this->viewMatrix;
}

void Ttn::camera::Camera::updateProjection(VkExtent2D newClipArea) {
  this->clipArea = newClipArea;
  this->computeProjectionMatrix();
}

const glm::mat4& Ttn::camera::Camera::getProjection() {
  return this->projMatrix;
}

void Ttn::camera::Camera::resetCamera() {
  this->worldPosition = this->initialWorldPosition;
  this->pointsTo = this->initialPointsTo;
  this->viewAngle = this->initialViewAngle;
  this->computeViewMatrix();
}

bool Ttn::camera::Camera::requireClick() {
  return this->options.mode == Ttn::camera::CameraMode::CAMERA_DRAG;
}
