#include <iostream>
#include <fstream>

#include <ttn/graphics/ttn_window.hpp>
#include <ttn/graphics/ttn_vulkan_app.hpp>
#include <ttn/shared/logger.hpp>

int main() {
  // std::ofstream logFile = ;
  // logFile.open("out.log", std::ofstream::out | std::ofstream::trunc);
  Ttn::Logger logger(&std::cout, Ttn::Logger::WithDatetime);

  try {
    Ttn::VulkanApp vulkanApp("Hello Tectonic", {800, 600, true}, 2, logger);
    vulkanApp.initialize();
    logger.flush();
    vulkanApp.run();
    vulkanApp.cleanUp();

  } catch (std::runtime_error const& e) {
    logger.Fatal(e.what());
  }

  
  return EXIT_SUCCESS;
}
