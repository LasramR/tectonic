#include "datetime.hpp"

std::string Ttn::datetime::getCurrentDateTimeString() {
  const auto now = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
  return std::format("{:%d-%m-%Y %H:%M:%OS}", now);
}
