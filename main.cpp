#include <iostream>
#include <fstream>

#include "ttn/graphics/ttn_window.hpp"
#include "ttn/graphics/ttn_vulkan_app.hpp"
#include "ttn/shared/logger.hpp"

int main() {
  std::ofstream logFile;
  logFile.open("out.log", std::ofstream::out | std::ofstream::trunc);
  Ttn::Logger logger(&logFile, Ttn::Logger::WithDatetime);
  Ttn::VulkanApp vulkanApp("Hello Tectonic", logger);
  try {
    vulkanApp.initialize({800, 600, true});
  } catch (std::runtime_error re) {
    logger.Fatal(re.what());
  }

  logger.flush();
  vulkanApp.run();
  vulkanApp.cleanUp();

  return EXIT_SUCCESS;
}
