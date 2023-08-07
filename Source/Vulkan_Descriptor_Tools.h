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
*/

#ifndef VULKAN_DESCRIPTOR_TOOLS_H
#define VULKAN_DESCRIPTOR_TOOLS_H

#include "Vulkan_Utils.h"
#include <stdint.h>

// Create descriptor set layout
VkResult CreateDescriptorSetLayout (VkDevice logicalDevice,
                                    VkDescriptorSetLayout* subpass0DescriptorSetLayout,
                                    VkDescriptorSetLayout* subpass1DescriptorSetLayout);
// Create Descriptor Pool
// 
// @Note: Since at any given time there may be one or more frames in flight, each swapchain image needs its own corresponding set of input attachments and other resources. (since their contents will be different for each swapchain image)
//           Because of this, we will need enough descriptors available in the pool to have a separate descriptor sets for each swapchain image we are using.
VkDescriptorPool CreateDescriptorPool (VkDevice logicalDevice,
                                       uint32_t numSwapchainImages);


// Creates pipeline that will be used by the first subpass
VkPipeline CreateSubpass0Pipeline(VkDevice               logicalDevice, 
                                  VkRenderPass           renderpass, 
                                  VkExtent2D*            pExtent, 
                                  VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                  const char*            fragShaderPath, 
                                  const char*            vertShaderPath,
                                  VkPipelineLayout*      pipelineLayoutHandleOut);

// Creates pipeline that will be used by the second subpass
VkPipeline CreateSubpass1Pipeline (VkDevice               logicalDevice,
                                   VkRenderPass           renderpass,
                                   VkExtent2D*            pExtent,
                                   VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                   const char*            fragShaderPath,
                                   const char*            vertShaderPath,
                                   VkPipelineLayout*      pPipelineLayoutHandleOut);

#endif // VULKAN_DESCRIPTOR_TOOLS_H
