#pragma once

#include <string>

#include "GLFW/glfw3.h"

namespace Ttn {

  struct Ttn_WindowProperties {
    int width;
    int height;
    bool resizable;
  };

  class Ttn_Window {
    private:
      std::string name;
      Ttn_WindowProperties windowProperties;
      GLFWwindow* window;

    public:
    Ttn_Window(std::string name, Ttn_WindowProperties windowProperties);
    ~Ttn_Window();
    
    bool ShouldClose();
  };

}