#include "datetime.hpp"
#include "logger.hpp"
#include "logger_level.hpp"

#include <chrono>
#include <format>

Ttn::Logger::Logger(std::ostream* outputStream, Ttn::Logger::Flag flags) : outputStream{outputStream}, flags{flags} {
  this->formatter = [&](LogLevel logLevel, std::string log) {
    std::string datetime = this->hasFlag(Ttn::Logger::WithDatetime) ? Ttn::datetime::getCurrentDateTimeString() : "";
    return std::format("[{}]{}:{}\n", LogLevelStr[logLevel], datetime, log);
  };
}

bool Ttn::Logger::hasFlag(Ttn::Logger::Flag flag) {
  return flag & this->flags == flag;
}


void Ttn::Logger::Info(std::string log) {
  if (this->outputStream != nullptr) {
    *outputStream << this->formatter(LogLevel::INFO, log);
  }
}

void Ttn::Logger::Warn(std::string log) {
  if (this->outputStream != nullptr) {
    *outputStream << this->formatter(LogLevel::WARN, log);
  }
}

void Ttn::Logger::Error(std::string log) {
  if (this->outputStream != nullptr) {
    *outputStream << this->formatter(LogLevel::ERROR, log);
  }
}

void Ttn::Logger::Fatal(std::string log) {
  if (this->outputStream != nullptr) {
    *outputStream << this->formatter(LogLevel::FATAL, log);
    exit(EXIT_FAILURE);
  }
}

void Ttn::Logger::flush() {
  if (this->outputStream != nullptr) {
    outputStream->flush();
  }
}
