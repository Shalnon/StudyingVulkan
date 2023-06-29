/* Copyright 2023 Sean Halnon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**  http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissionsand
**  limitations under the License.
**/

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
