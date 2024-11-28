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
      MousePos previousMousePos;
      MousePos currentMousePos;
      MouseBtn leftBtn;
      MouseBtn rightBtn;
      bool hasMoved();
      glm::vec2 getMoveDelta();
    } MouseState;

    class MouseStateListener {

      private:  
        GLFWwindow* window;
        MouseState state;
        static void mouseBtnCallback(GLFWwindow* window, int button, int action, int mods);
        static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
      public:
        MouseStateListener(GLFWwindow*);
        ~MouseStateListener();

        const MouseState consumeMouseState();
        void ignoreMouseMove();
    };
  }
}