#include "ttn_graphic_pipeline.hpp"

#include <format>

Ttn::pipelines::Ttn_Graphic_Pipeline::Ttn_Graphic_Pipeline(VkDevice vkDevice, Ttn::Logger& logger, Ttn::swapchain::Ttn_SwapChain& ttnSwapChain, Ttn::pipelines::Ttn_Renderpass& ttnRenderpass, Ttn::vertex::Ttn_Vertex_Buffer& ttnVertexBuffer, VkImageView textureView, VkSampler sampler, size_t maxFramesInFlight, Ttn::vertex::TtnVertex& ttnVertex) :
  vkDevice{vkDevice},
  logger{logger},
  ttnSwapChain{ttnSwapChain},
  ttnRenderpass{ttnRenderpass},
  ttnVertexBuffer{ttnVertexBuffer},
  maxFramesInFlight{maxFramesInFlight}
{
  auto shaderName = "shaders/triangle";
  this->logger.Info(std::format("Loading shader {}", shaderName));
  Ttn::shaders::Ttn_Shader ttnShader {this->vkDevice, shaderName};

  VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo {};
  vertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageCreateInfo.pName = "main";
  vertShaderStageCreateInfo.module = ttnShader.getVkVertShaderModule();

  VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo {};
  fragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageCreateInfo.pName = "main";
  fragShaderStageCreateInfo.module = ttnShader.getVkFragShaderModule();

  VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageCreateInfo, fragShaderStageCreateInfo};
  
  auto bindingDescription = ttnVertex.getBindingDescription();
  auto attributeDescriptions = ttnVertex.getAttributeDescriptions();

  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{};
  vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
  vertexInputStateCreateInfo.pVertexBindingDescriptions = &bindingDescription;
  vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
  vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

  this->dynamicStates = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
  };

  VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo {};
  dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(this->dynamicStates.size());
  dynamicStateCreateInfo.pDynamicStates = this->dynamicStates.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkViewport viewPort {};
  viewPort.x = 0.0f;
  viewPort.y = 0.0f;
  viewPort.width = (float) this->ttnSwapChain.getSwapChainExtent().width;
  viewPort.height = (float) this->ttnSwapChain.getSwapChainExtent().height;
  viewPort.minDepth = 0.0f;
  viewPort.maxDepth = 1.0f;

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = this->ttnSwapChain.getSwapChainExtent();

  VkPipelineViewportStateCreateInfo viewPortStateCreateInfo {};
  viewPortStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewPortStateCreateInfo.viewportCount = 1;
  viewPortStateCreateInfo.pViewports = &viewPort;
  viewPortStateCreateInfo.scissorCount = 1;
  viewPortStateCreateInfo.pScissors = &scissor;

  VkPipelineRasterizationStateCreateInfo rasterizer {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f; // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f; // Optional
  multisampling.pSampleMask = nullptr; // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE; // Optional

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f; // Optional
  colorBlending.blendConstants[1] = 0.0f; // Optional
  colorBlending.blendConstants[2] = 0.0f; // Optional
  colorBlending.blendConstants[3] = 0.0f; // Optional

  this->ttnUbo = new Ttn::pipelines::Ttn_UniformObjectBuffer(this->vkDevice, this->maxFramesInFlight);
  
  std::vector<VkDescriptorSetLayout> layouts(this->maxFramesInFlight, this->ttnUbo->layoutDescriptor);
  VkDescriptorSetAllocateInfo allocInfo {};
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = this->ttnUbo->descriptorPool;
  allocInfo.descriptorSetCount = static_cast<uint32_t>(this->maxFramesInFlight);
  allocInfo.pSetLayouts = layouts.data();

  this->descriptorSets.resize(this->maxFramesInFlight);
  if (vkAllocateDescriptorSets(this->vkDevice, &allocInfo, this->descriptorSets.data()) != VK_SUCCESS) {
    throw std::runtime_error("could not allocate descriptor sets");
  }

  for (size_t i = 0; i < this->maxFramesInFlight; i++) {
    VkDescriptorBufferInfo bufferInfo {};
    bufferInfo.buffer = this->ttnVertexBuffer.uniformBuffers[i];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(Ttn::pipelines::UniformBufferObject);

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = textureView;
    imageInfo.sampler = sampler;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = this->descriptorSets[i];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfo;

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = this->descriptorSets[i];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pImageInfo = &imageInfo;
    vkUpdateDescriptorSets(this->vkDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
  }

  VkPipelineDepthStencilStateCreateInfo depthStencil{};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.minDepthBounds = 0.0f;
  depthStencil.maxDepthBounds = 1.0f;
  depthStencil.stencilTestEnable = VK_FALSE;
  depthStencil.front = {};
  depthStencil.back = {};
  
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1; // Optional
  pipelineLayoutInfo.pSetLayouts = &ttnUbo->layoutDescriptor; // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

  if (vkCreatePipelineLayout(this->vkDevice, &pipelineLayoutInfo, nullptr, &this->pipelineLayout) != VK_SUCCESS) {
    throw std::runtime_error("could not create pipeline layout");
  }

  VkGraphicsPipelineCreateInfo graphicPipelineCreateInfo {};
  graphicPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  graphicPipelineCreateInfo.stageCount = 2;
  graphicPipelineCreateInfo.pStages = shaderStages;
  graphicPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
  graphicPipelineCreateInfo.pInputAssemblyState = &inputAssembly;
  graphicPipelineCreateInfo.pViewportState = &viewPortStateCreateInfo;
  graphicPipelineCreateInfo.pRasterizationState = &rasterizer;
  graphicPipelineCreateInfo.pMultisampleState = &multisampling;
  graphicPipelineCreateInfo.pDepthStencilState = &depthStencil;
  graphicPipelineCreateInfo.pColorBlendState = &colorBlending;
  graphicPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;

  graphicPipelineCreateInfo.layout = pipelineLayout;
  
  graphicPipelineCreateInfo.renderPass = this->ttnRenderpass.getRenderpass();
  graphicPipelineCreateInfo.subpass = 0;

  graphicPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
  graphicPipelineCreateInfo.basePipelineIndex = -1;

  if (vkCreateGraphicsPipelines(this->vkDevice, VK_NULL_HANDLE, 1, &graphicPipelineCreateInfo, nullptr, &this->pipeline) != VK_SUCCESS) {
    throw std::runtime_error("could not create graphic pipeline");
  };
}

Ttn::pipelines::Ttn_Graphic_Pipeline::~Ttn_Graphic_Pipeline() {
  delete this->ttnUbo;
  vkDestroyPipeline(this->vkDevice, this->pipeline, nullptr);
  vkDestroyPipelineLayout(this->vkDevice, this->pipelineLayout, nullptr);
}

VkPipeline Ttn::pipelines::Ttn_Graphic_Pipeline::getPipeline() {
  return this->pipeline;
}