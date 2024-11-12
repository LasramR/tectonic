#pragma once

#include <string>

enum LogLevel : u_int8_t {
  INFO,
  WARN,
  ERROR,
  FATAL
};

const std::string LogLevelStr[] = {"INFO", "WARNING", "ERROR", "FATAL"};