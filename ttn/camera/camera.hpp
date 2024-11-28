#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Ttn {

  namespace camera {

    enum CameraMode {
      CAMERA_DRAG = 0,
      CAMERA_CURSOR_CENTER = 1
    };

    enum CameraBehaviour {
      CAMERA_FOLLOW_POINTS_TO = 0,
      CAMERA_FOLLOW_POINTS_TO_INVERTED = 1,
    };
    
    typedef struct {
      CameraBehaviour behaviour;
      CameraMode mode;
      float sensitivity;
      float moveSpeed;
    } CameraOpts;

    CameraOpts DefaultCameraOpts();

    class Camera {

      private:
        GLFWwindow* window;
        CameraOpts options;

        glm::vec3 initialWorldPosition;
        glm::vec3 initialPointsTo;
        glm::vec2 initialViewAngle;
        
        VkExtent2D clipArea;
        glm::vec3 worldPosition;
        glm::vec2 viewAngle;
        glm::vec3 pointsTo;
        glm::vec3 upAngle;
        glm::mat4 viewMatrix;
        glm::mat4 projMatrix;

        void computeViewMatrix();
        void computeProjectionMatrix();

      public:
        Camera(GLFWwindow*, VkExtent2D, glm::vec3, glm::vec3, CameraOpts);
        Camera(GLFWwindow*, VkExtent2D, glm::vec3, CameraOpts);
        Camera(GLFWwindow*, VkExtent2D, CameraOpts);
        ~Camera();

        void moveAbsoluteWorldPosition(glm::vec3);
        void moveWorldHorizontalPositionRelativeToCameraAngle(glm::vec3);
        void moveWorldPositionRelativeToCameraAngle(glm::vec3);
        void moveViewAngle(glm::vec2);
        const glm::mat4& getViewSpace();
        void updateProjection(VkExtent2D);
        const glm::mat4& getProjection();
        void resetCamera();
        bool requireClick();
    };
  }
}