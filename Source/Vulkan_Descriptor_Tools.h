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

#endif // !VULKAN_DESCRIPTOR_TOOLS_H
