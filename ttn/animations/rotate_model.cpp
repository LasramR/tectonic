#include "rotate_model.hpp"

#include <cstring>

Ttn::animations::RotateModel::RotateModel(Ttn::vertex::Ttn_Vertex_Buffer& ttnVertexBuffer) :
  ttnVertexBuffer {ttnVertexBuffer},
  startingTime {std::chrono::high_resolution_clock::now()},
  lastTime {std::chrono::high_resolution_clock::now()},
  waitingStartingTime {std::chrono::high_resolution_clock::now()},
  isPlaying {false},
  acceleration {1.0f}
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
  Ttn::pipelines::UniformBufferObject ubo {};
  ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f) * this->acceleration, glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  ubo.proj = glm::perspective(glm::radians(45.0f), swapExtent.width / (float) swapExtent.height, 0.1f, 10.0f);
  ubo.proj[1][1] *= -1;
  memcpy(this->ttnVertexBuffer.uniformBufferMapped[ubIdx], &ubo, sizeof(ubo));

  lastTime = currentTime;
}