#pragma once

#include <vector>
#include <string>

#include <vulkan/vulkan.h>

namespace Ttn {

  namespace shaders {

    class Ttn_Shader {

      private:
        VkDevice vkDevice;

        std::vector<char> vertBuffer;
        std::vector<char> fragBuffer;
        VkShaderModule vkVertShaderModule;
        VkShaderModule vkFragShaderModule;

      public:
        Ttn_Shader(VkDevice, std::string);
        ~Ttn_Shader();

        VkShaderModule getVkVertShaderModule();
        VkShaderModule getVkFragShaderModule();
    };
  };

}