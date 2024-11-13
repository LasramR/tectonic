#pragma once

#include <string>

namespace Ttn {

  namespace shared {
  
    enum LogLevel : u_int8_t {
      INFO,
      WARN,
      ERROR,
      DEBUG,
      FATAL
    };

    const std::string LogLevelStr[] = {"INFO", "WARNING", "ERROR", "DEBUG", "FATAL"};

  }
}
