#pragma once

#include "logger_level.hpp"

#include <functional>
#include <iostream>
#include <string>

namespace Ttn {

  class Logger {
    typedef u_int8_t Flag;

    private:
      std::ostream* outputStream;
      u_int8_t flags;
      std::function<std::string(LogLevel, std::string)> formatter;

      bool hasFlag(Flag flag);

    public:
      static const Flag WithDatetime = 0b00000001;

      Logger(std::ostream* outputStream, Flag flags);
      void Info(std::string log);
      void Warn(std::string log);
      void Error(std::string log);
      /**
       * Log and exit programm with EXIT_FAILURE
       */
      void Fatal(std::string log);
      void flush();
  };

}