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
    Ttn::VulkanApp vulkanApp("Hello Tectonic", logger);
    vulkanApp.initialize({800, 600, true});
    logger.flush();
    vulkanApp.run();
    vulkanApp.cleanUp();

  } catch (std::runtime_error const& e) {
    logger.Fatal(e.what());
  }

  
  return EXIT_SUCCESS;
}
