#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

Ttn::camera::Camera::Camera(VkExtent2D clipArea, glm::vec3 worldPosition, glm::vec3 viewAngle) :
  clipArea {clipArea},
  worldPosition {worldPosition},
  viewAngle {viewAngle},
  upAngle {0.0f, 0.0f, 1.0f}
{
  this->computeViewMatrix();
  this->computeProjectionMatrix();
}

Ttn::camera::Camera::Camera(VkExtent2D clipArea, glm::vec3 worldPosition) : 
Camera(
  clipArea,
  worldPosition,
  {0.0f, 0.0f, 1.0f}
) {}

Ttn::camera::Camera::Camera(VkExtent2D clipArea) : 
Camera(
  clipArea,
  {2.0f, 2.0f, 2.0f},
  {0.0f, 0.0f, 1.0f}
) {}


Ttn::camera::Camera::~Camera() {

}

void Ttn::camera::Camera::computeViewMatrix() {
  this->viewMatrix = glm::lookAt(this->worldPosition, this->viewAngle, this->upAngle);
}

void Ttn::camera::Camera::computeProjectionMatrix() {
  // glm::perspective(FOV, aspect ratio, how close an object can be to the camera, how far can on object be to the camera)
  this->projMatrix = glm::perspective(glm::radians(45.0f), this->clipArea.width / (float) this->clipArea.height, 0.1f, 1000.0f);
  this->projMatrix[1][1] *= -1;
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