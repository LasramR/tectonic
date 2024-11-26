#include <iostream>
#include <fstream>

#include <ttn/graphics/ttn_window.hpp>
#include <ttn/graphics/ttn_vulkan_app.hpp>
#include <ttn/shared/logger.hpp>

int main(int argc, char* argv[]) {
  // std::ofstream logFile = ;
  // logFile.open("out.log", std::ofstream::out | std::ofstream::trunc);
  Ttn::Logger logger(&std::cout, Ttn::Logger::WithDatetime);

  if (argc != 1 && argc != 3) {
    logger.Fatal("wrong number of argument, usage should either be ./tectonic or ./tectonic <object_file_path> <texture_file_path>");
  }

  const char* modelPath = argc == 1 ? "objects/viking_room.obj" : argv[1];
  const char* texturePath = argc == 1 ? "textures/viking_room.png" : argv[2];

  try {
    Ttn::VulkanApp vulkanApp("Hello Tectonic", {800, 600, true}, 2, modelPath, texturePath, logger);
    vulkanApp.initialize();
    logger.flush();
    vulkanApp.run();
    vulkanApp.cleanUp();

  } catch (std::runtime_error const& e) {
    logger.Fatal(e.what());
  }

  
  return EXIT_SUCCESS;
}
