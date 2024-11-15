#include "ttn_shader.hpp"

#include <format>
#include <fstream>
#include <stdexcept>

Ttn::shaders::Ttn_Shader::Ttn_Shader(VkDevice vkDevice, std::string shaderName) :
  vkDevice{vkDevice},
  vkVertShaderModule{VK_NULL_HANDLE},
  vkFragShaderModule{VK_NULL_HANDLE}
{
  std::ifstream vertFile(std::format("{}.vert.spv", shaderName), std::ios::ate | std::ios::binary);
  if (!vertFile.is_open()) {
    throw std::runtime_error(std::format("failed to open {}.vert.spv file", shaderName));
  }

  std::ifstream fragFile(std::format("{}.frag.spv", shaderName), std::ios::ate | std::ios::binary);
  if (!fragFile.is_open()) {
    throw std::runtime_error(std::format("failed to open {}.frag.spv file", shaderName));
  }

  size_t vertFileSize = (size_t) vertFile.tellg();
  this->vertBuffer.resize(vertFileSize);
  vertFile.seekg(0);
  vertFile.read(this->vertBuffer.data(), vertFileSize);
  vertFile.close();

  size_t fragFileSize = (size_t) fragFile.tellg();
  this->fragBuffer.resize(fragFileSize);
  fragFile.seekg(0);
  fragFile.read(this->fragBuffer.data(), fragFileSize);
  fragFile.close();

  VkShaderModuleCreateInfo vertShaderModuleCreateInfo {};
  vertShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  vertShaderModuleCreateInfo.codeSize = this->vertBuffer.size();
  vertShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(this->vertBuffer.data());

  VkShaderModuleCreateInfo fragShaderModuleCreateInfo {};
  fragShaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  fragShaderModuleCreateInfo.codeSize = this->fragBuffer.size();
  fragShaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(this->fragBuffer.data());

  if (vkCreateShaderModule(this->vkDevice, &vertShaderModuleCreateInfo, nullptr, &this->vkVertShaderModule) != VK_SUCCESS) {
    throw std::runtime_error("could not create vertex shader module");
  }

  if (vkCreateShaderModule(this->vkDevice, &fragShaderModuleCreateInfo, nullptr, &this->vkFragShaderModule) != VK_SUCCESS) {
    throw std::runtime_error("could not create vertex shader module");
  }
}

Ttn::shaders::Ttn_Shader::~Ttn_Shader() {
  vkDestroyShaderModule(this->vkDevice, this->vkVertShaderModule, nullptr);
  vkDestroyShaderModule(this->vkDevice, this->vkFragShaderModule, nullptr);
}

VkShaderModule Ttn::shaders::Ttn_Shader::getVkVertShaderModule() {
  return this->vkVertShaderModule;
}
VkShaderModule Ttn::shaders::Ttn_Shader::getVkFragShaderModule() {
  return this->vkFragShaderModule;
}
