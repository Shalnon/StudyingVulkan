#ifndef VULKAN_BUFFER_TOOLS_H
#define VULKAN_BUFFER_TOOLS_H
#include "Vulkan_Utils.h"
#include <glm\glm.hpp>

// used to map data to the vertex attributes
struct VertexAttributeData
{
    float position[3];
    float normal[3];
};

struct AttributeInfo
{
    uint32_t bufferBindingIdx; // VkVertexInputAttributeDescription::binding
    uint32_t attributeIdx;     // VkVertexInputAttributeDescription::location
    uint32_t stride;           // Stride of the attribute. (not the entire vertex)
    uint32_t offset;           // Attribute offset - this will be non-0 when using interleaved attributes. (as in position and normal for a vertex being next to each other in the buffer)
    VkFormat format;           // The format of the data : determines which data goes to which xyzw component
};

#define VERTEX_ATTRIB_POSITION_IDX SceneVulkanParameters::Subpass0::vertexLocationAttributePosition
#define VERTEX_ATTRIB_NORMAL_IDX SceneVulkanParameters::Subpass0::vertexNormalAttributePosition

// All attributes will come from the same buffer for now
static AttributeInfo s_VertexShaderAttributes[] =
{
    { // POSITION ATTRIBUTE
        /*...uint32_t...bufferBindingIdx..*/ 0,
        /*...uint32_t...attributeIdx......*/ SceneVulkanParameters::Subpass0::vertexLocationAttributePosition, //0
        /*...uint32_t...stride............*/ sizeof (VertexAttributeData::position),
        /*...uint32_t...offset............*/ offsetof (VertexAttributeData, position),
        /*...VkFormat...format............*/ VK_FORMAT_R32G32B32_SFLOAT
    },
    { // VERTEX NORMAL ATTRIBUTE
        /*...uint32_t...bufferBindingIdx..*/ 0,
        /*...uint32_t...attributeIdx......*/ SceneVulkanParameters::Subpass0::vertexNormalAttributePosition, //1
        /*...uint32_t...stride............*/ sizeof (VertexAttributeData::normal),
        /*...uint32_t...offset............*/ offsetof (VertexAttributeData, normal),
        /*...VkFormat...format............*/ VK_FORMAT_R32G32B32_SFLOAT
    }
};

// Struct for tracking per-mesh data
struct MeshInfo
{
    uint32_t           firstPrimIdx;
    uint32_t           numPrims;
    uint32_t           materialIdx;
    glm::mat4x4        modelMatrix;
    VkAabbPositionsKHR aabb; // AABB for the individual mesh
};

// non-api struct we will use to track info needed to work with a specific VkBuffer and its data.
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
    uint32_t                  numMeshes;

    const MeshInfo*           pMeshes;
    // Using the VK sceneAABB struct which is a part of VK_KHR_acceleration_structure extenction,
    //      but were just using it as a generic sceneAABB tracking struct.
    VkAabbPositionsKHR sceneAABB; // AABB for all geometry
};

// light-weight inline function for mapping buffer memory and getting a cpu ptr to it.
inline void* MapBufferMemory (vulkanAllocatedBufferInfo bufferInfo,
                              VkDevice                  logicalDevice)
{
    void* pMem = nullptr;
    vkMapMemory (/*...VkDevice...........device...*/ logicalDevice,
                 /*...VkDeviceMemory.....memory...*/ bufferInfo.memoryHandle,
                 /*...VkDeviceSize.......offset...*/ bufferInfo.offset,
                 /*...VkDeviceSize.......size.....*/ bufferInfo.buffersize,
                 /*...VkMemoryMapFlags...flags....*/ 0, //@spec:  "reserved for future use"
                 /*...void**.............ppData...*/ &pMem);

    assert (pMem);

    return pMem;
}

VkDeviceMemory AllocateVkBufferMemory (VkPhysicalDevice      physicalDevice,
                                       VkDevice              logicalDevice,
                                       VkMemoryPropertyFlags requiredPropertyFlags, // ex: a mask of VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, etc...
                                       VkMemoryRequirements* pBufferMemoryRequirements);

vulkanAllocatedBufferInfo CreateAndAllocaStagingBuffer (VkPhysicalDevice physicalDevice,
                                                        VkDevice         logicalDevice,
                                                        uint32_t         bufferSizeInBytes,
                                                        uint32_t         queueIndex);


inline vulkanAllocatedBufferInfo CreateAndAllocateVertexBuffer (VkPhysicalDevice physicalDevice,
                                                                VkDevice         logicalDevice,
                                                                uint32_t         bufferSizeInBytes,
                                                                uint32_t         queueIndex);
 
inline vulkanAllocatedBufferInfo CreateAndAllocateIndexBuffer (VkPhysicalDevice physicalDevice,
                                                               VkDevice         logicalDevice,
                                                               uint32_t         bufferSizeInBytes,
                                                               uint32_t         queueIndex);

inline vulkanAllocatedBufferInfo CreateAndAllocateUniformBuffer (VkPhysicalDevice physicalDevice,
                                                                 VkDevice         logicalDevice,
                                                                 uint32_t         bufferSizeInBytes,
                                                                 uint32_t         queueIndex);

inline vulkanAllocatedBufferInfo CreateAndAllocateSsbo (VkPhysicalDevice physicalDevice,
                                                        VkDevice         logicalDevice,
                                                        uint32_t         bufferSizeInBytes,
                                                        uint32_t         queueIndex);
 
void ExecuteBuffer2BufferCopy (VkPhysicalDevice          physicalDevice,
                               VkDevice                  logicalDevice,
                               VkQueue                   queue,
                               uint32_t                  queueFamilyIndex,
                               VkDeviceSize              copySize,
                               vulkanAllocatedBufferInfo srcBufferInfo,
                               vulkanAllocatedBufferInfo dstBufferInfo);

// prototype aiScene* ptr definition for use in the function signature below. without it we'd have to include an assimp header.
struct aiScene;

// Loads vertex data from a file into a buffer backed by device local memory.
GeometryBufferSet CreateGeometryBuffersAndAABBs (VkPhysicalDevice    physicalDevice, 
                                                 VkDevice            logicalDevice,
                                                 VkQueue             queue,
                                                 uint32_t            queueFamilyIndex,
                                                 const aiScene*      pScene);

vulkanAllocatedBufferInfo CreateMeshColorsStorageBuffer (VkPhysicalDevice    physicalDevice,
                                                         VkDevice            logicalDevice,
                                                         VkQueue             queue,
                                                         uint32_t            queueFamilyIndex,
                                                         const aiScene*      pScene);

// Creates and initilizes UBO and fills it with data
vulkanAllocatedBufferInfo CreateUniformBuffer (VkPhysicalDevice             physicalDevice,
                                               VkDevice                     logicalDevice,
                                               VkQueue                      queue,
                                               uint32_t                     queueFamilyIndex,
                                               const GeometryBufferSet*     pGeometryBufferSet,
                                               VkAabbPositionsKHR*          pDesiredSceneBounds,
                                               bool                         maintainAspectRatio);
#endif