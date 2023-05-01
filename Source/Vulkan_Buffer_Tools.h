#pragma once
#ifndef VULKAN_BUFFER_TOOLS_H
#define VULKAN_BUFFER_TOOLS_H

#include "StudyingVulkan.h"//#include "Vulkan_Utils.h"
#include "Asset_Tools.h"

#define VERTEX_ATTRIB_POSITION_IDX 0

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
    VkDeviceSize   buffersize;
    uint32_t       offset;
};

// Tracks a usable vertex buffer - index buffer pair. May also be a good place to add things like acceleration structure tracking as well.
struct GeometryBufferSet
{
    vulkanAllocatedBufferInfo vertexBufferInfo;
    vulkanAllocatedBufferInfo indexBufferInfo;
    uint32_t                  numVertices;
    uint32_t                  numTriangles;
};

inline void* MapBufferMemory (vulkanAllocatedBufferInfo bufferInfo,
                              VkDevice                  logicalDevice)
{
    void* pMem = nullptr;
    vkMapMemory (/*...VkDevice...........device.....*/ logicalDevice,
                 /*...VkDeviceMemory.....memory.....*/ bufferInfo.memoryHandle,
                 /*...VkDeviceSize.......offset.....*/ bufferInfo.offset,
                 /*...VkDeviceSize.......size.......*/ bufferInfo.buffersize,
                 /*...VkMemoryMapFlags...flags......*/ 0,
                 /*...void**.............ppData.....*/ &pMem);

    assert (pMem);

    return pMem;
}


void GetMemoryType (VkPhysicalDevice      physicalDevice,
                    VkMemoryPropertyFlags requiredPropertyFlags, // ex: a mask of VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, etc...
                    uint32_t*             pChosenMemTypeIdxOut,
                    VkMemoryRequirements* memRequirements) ;

VkDeviceMemory AllocateVkBufferMemory (VkPhysicalDevice      physicalDevice,
                                       VkDevice              logicalDevice,
                                       VkMemoryPropertyFlags requiredPropertyFlags, // ex: a mask of VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, etc...
                                       VkMemoryRequirements* pBufferMemoryRequirements);


vulkanAllocatedBufferInfo CreateStagingBuffer (VkPhysicalDevice physicalDevice,
                                               VkDevice         logicalDevice,
                                               uint32_t         bufferSizeInBytes,
                                               uint32_t         queueIndex);

vulkanAllocatedBufferInfo CreateAndAllocateVertexBuffer (VkPhysicalDevice physicalDevice,
                                                         VkDevice         logicalDevice,
                                                         uint32_t         bufferSizeInBytes,
                                                         uint32_t         queueIndex);

vulkanAllocatedBufferInfo CreateAndAllocateIndexBuffer (VkPhysicalDevice physicalDevice,
                                                        VkDevice         logicalDevice,
                                                        uint32_t         bufferSizeInBytes,
                                                        uint32_t         queueIndex);

VkResult ExecuteBuffer2BufferCopy (VkPhysicalDevice          physicalDevice,
                                   VkDevice                  logicalDevice,
                                   VkQueue                   queue,
                                   uint32_t                  queueFamilyIndex,
                                   VkDeviceSize              copySize,
                                   vulkanAllocatedBufferInfo srcBufferInfo,
                                   vulkanAllocatedBufferInfo dstBufferInfo);

// Loads vertex data from a file into a buffer backed by device local memory.
GeometryBufferSet CreateGeometryBuffersFromAiScene (VkPhysicalDevice physicalDevice, // @TODO, Create ssbo (or ubo maybe) with individual transform matrices for files which define multiple geometries/objects/meshes/whatever you want to call them
                                                 VkDevice         logicalDevice,
                                                 VkQueue          queue,
                                                 uint32_t         queueFamilyIndex,
                                                 const aiScene*   pScene,
                                                 bool             useInterleavedAttributes);
#endif