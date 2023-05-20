#ifndef VULKAN_DESCRIPTOR_TOOLS_H
#define VULKAN_DESCRIPTOR_TOOLS_H
#include "Vulkan_Utils.h"
#include <glm\glm.hpp>

// Create descriptor set layout
VkDescriptorSetLayout CreateDescriptorSetLayout (VkDevice logicalDevice);

// Create descriptor pool
VkDescriptorPool CreateDescriptorPool (VkDevice logicalDevice);

void UpdateMeshTransformUbo (VkDevice                logicalDevice,
                             VkDescriptorBufferInfo* pTransformBufferDescriptorInfo,
                             VkDescriptorSet         descriptorSet);

VkDescriptorSet AllocateDescriptorSet (VkDevice               logicalDevice,
                                       VkDescriptorPool       descriptorPool,
                                       VkDescriptorSetLayout  descriptorSetLayout);

#endif // !VULKAN_DESCRIPTOR_TOOLS_H
