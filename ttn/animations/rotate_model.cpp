#include "rotate_model.hpp"

#include <cstring>

Ttn::animations::RotateModel::RotateModel(Ttn::vertex::Ttn_Vertex_Buffer& ttnVertexBuffer, Ttn::camera::Camera& camera) :
  ttnVertexBuffer {ttnVertexBuffer},
  startingTime {std::chrono::high_resolution_clock::now()},
  lastTime {std::chrono::high_resolution_clock::now()},
  waitingStartingTime {std::chrono::high_resolution_clock::now()},
  isPlaying {false},
  acceleration {1.0f},
  camera {camera}
{}

Ttn::animations::RotateModel::~RotateModel() {}

void Ttn::animations::RotateModel::resetAnimation() {
  this->startingTime = std::chrono::high_resolution_clock::now();
  this->lastTime = std::chrono::high_resolution_clock::now();
  this->waitingStartingTime = std::chrono::high_resolution_clock::now();
  this->acceleration = 1.0f;
}

void Ttn::animations::RotateModel::start() {
  this->resetAnimation();
  this->isPlaying = true;
}

void Ttn::animations::RotateModel::decreaseAcceleration() {
  this->acceleration -= this->acceleration * 0.1f;
}

void Ttn::animations::RotateModel::increaseAcceleration() {
  this->acceleration += this->acceleration * 0.1f;
}

void Ttn::animations::RotateModel::toggleAnimation() {
  this->isPlaying = !this->isPlaying;

  if (this->isPlaying) {
    auto currentTime = std::chrono::high_resolution_clock::now();
    this->startingTime = this->startingTime + (currentTime - this->waitingStartingTime);
    lastTime = currentTime;
  } else {
    this->waitingStartingTime = std::chrono::high_resolution_clock::now();
  }
}

void Ttn::animations::RotateModel::updateUniformBuffer(VkExtent2D swapExtent,uint32_t ubIdx) {
  auto currentTime = this->isPlaying ? std::chrono::high_resolution_clock::now() : this->lastTime;

  float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - this->startingTime).count();
  
  // TBH I will need to check what the actual fuck is these following lines doing
  // edit now I understand
  Ttn::pipelines::UniformBufferObject ubo {};
  ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) * this->acceleration, glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = this->camera.getViewSpace();
  ubo.proj = this->camera.getProjection();
  memcpy(this->ttnVertexBuffer.uniformBufferMapped[ubIdx], &ubo, sizeof(ubo));

  lastTime = currentTime;
}