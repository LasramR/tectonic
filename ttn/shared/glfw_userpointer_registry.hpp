#pragma once

#include <ttn/graphics/ttn_window.hpp>
#include <ttn/input/keyboard_input.hpp>

namespace Ttn {
  namespace shared {
    class GlfwUserPointerRegistry {

      public:
        GlfwUserPointerRegistry();
        ~GlfwUserPointerRegistry();
        Ttn::Ttn_Window* ttnWindow;
        Ttn::input::KeyboardInputListener* keyboardInputListener;
    };
  }
}