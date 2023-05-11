#pragma once
#include "Vulkan_Utils.h"

void GetMemoryType (VkPhysicalDevice      physicalDevice,
                    VkMemoryPropertyFlags requiredPropertyFlags, // ex: a mask of VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, etc...
                    uint32_t* pChosenMemTypeIdxOut,
                    VkMemoryRequirements* memRequirements);

VkDeviceMemory AllocateVkBufferMemory (VkPhysicalDevice      physicalDevice,
                                       VkDevice              logicalDevice,
                                       VkMemoryPropertyFlags requiredPropertyFlags, // ex: a mask of VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, etc...
                                       VkMemoryRequirements* pBufferMemoryRequirements);



void CreateVertexBuffer (VkPhysicalDevice physicalDevice,
                         VkDevice         logicalDevice,
                         uint32_t         bufferSizeInBytes,
                         uint32_t         queueIndex,
                         VkQueue          queue,
                         void* pVertexData,
                         VkBuffer* pVertexBufferOut,
                         VkDeviceMemory* pVertexBufferMemoryOut);

// used to map data to the vertex attributes
struct VertexAttributeData
{
    float position[3];
};

struct AttributeInfo
{
    uint32_t bufferBindingIdx; // VkVertexInputAttributeDescription::binding
    uint32_t attributeIdx;     // VkVertexInputAttributeDescription::location
    uint32_t stride;           // Stride of the attribute. (not the entire vertex)
    uint32_t offset;           // Attribute offset - this will be non-0 when using interleaved attributes. (as in position and normal for a vertex being next to each other in the buffer)
    VkFormat format;           // The format of the data : determines which data goes to which xyzw component
};

#define VERTEX_ATTRIB_POSITION_IDX 0

// All attributes will come from the same buffer for now
static AttributeInfo s_VertexShaderAttributes[] =
{
    { // POSITION ATTRIBUTE
        /*...uint32_t...bufferBindingIdx..*/ 0,
        /*...uint32_t...attributeIdx......*/ 0,
        /*...uint32_t...stride............*/ sizeof (VertexAttributeData::position),
        /*...uint32_t...offset............*/ offsetof (VertexAttributeData, position),
        /*...VkFormat...format............*/ VK_FORMAT_R32G32B32_SFLOAT
    }
};

struct vulkanAllocatedBufferInfo
{
    VkBuffer       bufferHandle;
    VkDeviceMemory memoryHandle;
    uint32_t       buffersize;
    uint32_t       offset;
};

inline vulkanAllocatedBufferInfo CreateStagingBuffer (VkPhysicalDevice physicalDeviceHandle,
                                                      VkDevice         logicalDevice,
                                                      uint32_t         bufferSizeInBytes,
                                                      uint32_t         queueIndex)
{



    return {};
}