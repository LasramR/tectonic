#pragma once

#include <optional>

#include <GLFW/glfw3.h>

namespace Ttn {
  namespace input {

    typedef struct {
      int key;
      bool isPressed;
      bool isRepeted;
    } KeyboardInput;

    class KeyboardInputListener {
      private:
        GLFWwindow* window;
        std::optional<KeyboardInput> currentKeyboardInput;
        static void keyboardEventCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        
        public:
          KeyboardInputListener(GLFWwindow*);
          ~KeyboardInputListener();

          std::optional<KeyboardInput> consumeCurrentKeyboardInput();
    };
  }
}