#pragma once

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Ttn {

  namespace camera {

    class Camera {

      private:
        VkExtent2D clipArea;
        glm::vec3 worldPosition;
        glm::vec3 viewAngle;
        glm::vec3 upAngle;
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;

        void computeViewMatrix();
        void computeProjectionMatrix();

      public:
        Camera(VkExtent2D, glm::vec3, glm::vec3);
        Camera(VkExtent2D, glm::vec3);
        Camera(VkExtent2D);
        ~Camera();

        const glm::mat4& getViewSpace();
        void updateProjection(VkExtent2D);
        const glm::mat4& getProjection();
    };
  }
}