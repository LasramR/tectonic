#pragma once

#include <chrono>

#include <ttn/vertex/ttn_vertex_buffer.hpp>
#include <ttn/pipelines/ttn_uniform_object_buffer.hpp>

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Ttn {

  namespace animations {

    class RotateModel {

      private:
        std::chrono::high_resolution_clock::time_point startingTime;
        std::chrono::high_resolution_clock::time_point lastTime;
        std::chrono::high_resolution_clock::time_point waitingStartingTime;

        bool isPlaying;
        float acceleration;

        Ttn::vertex::Ttn_Vertex_Buffer& ttnVertexBuffer;

      public:
        RotateModel(Ttn::vertex::Ttn_Vertex_Buffer&);
        ~RotateModel();

        void resetAnimation();
        void start();
        void decreaseAcceleration();
        void increaseAcceleration();
        void toggleAnimation();
        void updateUniformBuffer(VkExtent2D, uint32_t);
    };

  }
}