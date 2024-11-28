#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Ttn {

  namespace input {

    typedef struct {
      double xpos;
      double ypos;
    } MousePos;

    typedef struct {
      bool isClicked;
      bool isReleased;
    } MouseBtn;

    typedef struct {
      bool hasBeenCentered;
      MousePos previousMousePos;
      MousePos currentMousePos;
      MouseBtn leftBtn;
      MouseBtn rightBtn;
      bool hasMoved();
      glm::vec2 getMoveDelta();
    } MouseState;

    enum MouseConstraint : uint16_t {
      NO_CONSTRAINT = 1 << 0,
      ALWAYS_CENTERED = 1 << 1,
      HIDDEN_CURSOR = 1 << 2
    };

    typedef uint32_t MouseConstraintFlags;

    MouseConstraint DefaultMouseConstraint();

    class MouseStateListener {

      private:  
        GLFWwindow* window;
        MouseConstraintFlags constraints;
        MouseState state;
        static void mouseBtnCallback(GLFWwindow* window, int button, int action, int mods);
        static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
      public:
        MouseStateListener(GLFWwindow*, MouseConstraintFlags);
        ~MouseStateListener();

        const MouseState consumeMouseState();
        void ignoreMouseMove();
    };
  }
}