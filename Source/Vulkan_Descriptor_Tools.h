#ifndef VULKAN_DESCRIPTOR_TOOLS_H
#define VULKAN_DESCRIPTOR_TOOLS_H

#include "Vulkan_Utils.h"
#include <glm\glm.hpp>

// Create descriptor set layout
VkResult CreateDescriptorSetLayout (VkDevice logicalDevice,
                                    VkDescriptorSetLayout* subpass0DescriptorSetLayout,
                                    VkDescriptorSetLayout* subpass1DescriptorSetLayout);

// Create descriptor pool
VkDescriptorPool CreateDescriptorPool (VkDevice logicalDevice);



VkPipeline CreateSubpass0Pipeline(VkDevice               logicalDevice, 
                                  VkRenderPass           renderpass, 
                                  VkExtent2D*            pExtent, 
                                  VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                  const char*            fragShaderPath, 
                                  const char*            vertShaderPath,
                                  VkPipelineLayout*      pipelineLayoutHandleOut);

VkPipeline CreateSubpass1Pipeline (VkDevice               logicalDevice,
                                   VkRenderPass           renderpass,
                                   VkExtent2D*            pExtent,
                                   VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                   const char*            fragShaderPath,
                                   const char*            vertShaderPath,
                                   VkPipelineLayout*      pPipelineLayoutHandleOut);

#endif // VULKAN_DESCRIPTOR_TOOLS_H
