#ifndef VULKAN_BUFFER_TOOLS_CPP
#define VULKAN_BUFFER_TOOLS_CPP

#include "Vulkan_Buffer_Tools.h"
#include "assimp/scene.h"
#include "Asset_Tools.h"
#include "Vulkan_Utils.h"
#include "Logging.h"
#include <assimp/material.h>
#include <glm/gtc/color_space.hpp>

VkDeviceMemory AllocateVkBufferMemory (VkPhysicalDevice      physicalDevice,
                                       VkDevice              logicalDevice,
                                       VkMemoryPropertyFlags requiredPropertyFlags,
                                       VkMemoryRequirements* pBufferMemoryRequirements)
{
    uint32_t             memTypeIndex = ChooseMemoryTypeIdx (physicalDevice, requiredPropertyFlags, pBufferMemoryRequirements);
    VkMemoryAllocateInfo allocateInfo =
    {
        /*...VkStructureType....sType..............*/ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        /*...const.void*........pNext..............*/ 0,
        /*...VkDeviceSize.......allocationSize.....*/ pBufferMemoryRequirements->size,
        /*...uint32_t...........memoryTypeIndex....*/ memTypeIndex
    };

    VkDeviceMemory deviceMem = VK_NULL_HANDLE;
    VkResult       result    = vkAllocateMemory (logicalDevice, &allocateInfo, nullptr, &deviceMem);

    assert (result == VK_SUCCESS);
    assert (deviceMem != VK_NULL_HANDLE);

    return deviceMem;
}

inline vulkanAllocatedBufferInfo CreateAndAllocateVertexBuffer (VkPhysicalDevice physicalDevice,
                                                                VkDevice         logicalDevice,
                                                                uint32_t         bufferSizeInBytes,
                                                                uint32_t         queueIndex)
{
    // Create vertex buffer
    VkBuffer           vertexBuffer           = VK_NULL_HANDLE;
    VkBufferUsageFlags vertexBufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    VkBufferCreateInfo vertexBufferCreateInfo =
     {
         /*...VkStructureType........sType.....................*/ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
         /*...const.void*............pNext.....................*/ 0, //reminder: buffer device address struct would need to go here if using that feature
         /*...VkBufferCreateFlags....flags.....................*/ 0,
         /*...VkDeviceSize...........size......................*/ bufferSizeInBytes,
         /*...VkBufferUsageFlags.....usage.....................*/ vertexBufferUsageFlags,
         /*...VkSharingMode..........sharingMode...............*/ VK_SHARING_MODE_EXCLUSIVE,
         /*...uint32_t...............queueFamilyIndexCount.....*/ 1,
         /*...const.uint32_t*........pQueueFamilyIndices.......*/ &queueIndex
     };
 
    //Create the empty/unallocated/yet-to-be-backed-by-memory buffer object that will hold the position data and get the handle
    VkResult result = vkCreateBuffer (logicalDevice, &vertexBufferCreateInfo, 0, &vertexBuffer);

    VkMemoryRequirements vertexBufferMemRequirements = {};
    vkGetBufferMemoryRequirements (logicalDevice, vertexBuffer, &vertexBufferMemRequirements);

    // Allocate device local memory that will hold the vertex data to be accessed from the gpu
    VkDeviceMemory vertexMem = AllocateVkBufferMemory (physicalDevice,
                                                       logicalDevice,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                       &vertexBufferMemRequirements);

    //bind the vertex buffer memory
    vkBindBufferMemory (logicalDevice, vertexBuffer, vertexMem, 0);

    return {/*...VkBuffer.......bufferHandle......*/ vertexBuffer,
            /*...VkDeviceMemory.memoryHandle......*/ vertexMem,
            /*...VkDeviceSize...buffersize........*/ vertexBufferMemRequirements.size,
            /*...uint32_t.......offset............*/ 0 };
}

///@TODO: Make this functions inline
inline vulkanAllocatedBufferInfo CreateAndAllocateIndexBuffer (VkPhysicalDevice physicalDevice,
                                                               VkDevice         logicalDevice,
                                                               uint32_t         bufferSizeInBytes,
                                                               uint32_t         queueIndex)
{
    VkBuffer           indexBuffer           = VK_NULL_HANDLE;
    VkBufferUsageFlags indexBufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    VkBufferCreateInfo indexBufferCreateInfo =
     {
         /*...VkStructureType........sType.....................*/ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
         /*...const.void*............pNext.....................*/ 0, //reminder: buffer device address struct would need to go here if using that feature
         /*...VkBufferCreateFlags....flags.....................*/ 0,
         /*...VkDeviceSize...........size......................*/ bufferSizeInBytes,
         /*...VkBufferUsageFlags.....usage.....................*/ indexBufferUsageFlags,
         /*...VkSharingMode..........sharingMode...............*/ VK_SHARING_MODE_EXCLUSIVE,
         /*...uint32_t...............queueFamilyIndexCount.....*/ 1,
         /*...const.uint32_t*........pQueueFamilyIndices.......*/ &queueIndex
     };

    VkResult result = vkCreateBuffer (logicalDevice, &indexBufferCreateInfo, 0, &indexBuffer);

    VkMemoryRequirements indexBufferMemRequirements = {};
    vkGetBufferMemoryRequirements (logicalDevice, indexBuffer, &indexBufferMemRequirements);

    // Allocate device local memory that will hold the vertex data to be accessed from the gpu
    VkDeviceMemory indexMem = AllocateVkBufferMemory (physicalDevice, logicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &indexBufferMemRequirements);

     //bind the index buffer memory
    vkBindBufferMemory (logicalDevice, indexBuffer, indexMem, 0);

    return {/*...VkBuffer.......bufferHandle......*/ indexBuffer,
            /*...VkDeviceMemory.memoryHandle......*/ indexMem,
            /*...VkDeviceSize...buffersize........*/ indexBufferMemRequirements.size,
            /*...uint32_t.......offset............*/ 0 };
}

inline vulkanAllocatedBufferInfo CreateAndAllocateSsbo (VkPhysicalDevice physicalDevice,
                                                        VkDevice         logicalDevice,
                                                        uint64_t         bufferSizeInBytes,
                                                        uint32_t         queueIndex)
{
    VkBuffer           storageBufferHandle = VK_NULL_HANDLE;
    VkBufferUsageFlags storageBufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    VkBufferCreateInfo storageBufferCreateInfo =
    {
        /*...VkStructureType........sType.....................*/ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        /*...const.void*............pNext.....................*/ 0, //reminder: buffer device address struct would need to go here if using that feature
        /*...VkBufferCreateFlags....flags.....................*/ 0,
        /*...VkDeviceSize...........size......................*/ bufferSizeInBytes,
        /*...VkBufferUsageFlags.....usage.....................*/ storageBufferUsageFlags,
        /*...VkSharingMode..........sharingMode...............*/ VK_SHARING_MODE_EXCLUSIVE,
        /*...uint32_t...............queueFamilyIndexCount.....*/ 1,
        /*...const.uint32_t*........pQueueFamilyIndices.......*/ &queueIndex
    };

    VkResult result = vkCreateBuffer (logicalDevice, &storageBufferCreateInfo, 0, &storageBufferHandle);

    VkMemoryRequirements storageBufferMemRequirements = {};
    vkGetBufferMemoryRequirements (logicalDevice, storageBufferHandle, &storageBufferMemRequirements);

#ifdef DEBUG
    assert (storageBufferMemRequirements.alignment <= SceneVulkanParameters::numBytesPerColor);
    if (storageBufferMemRequirements.alignment < SceneVulkanParameters::numBytesPerColor)
    {
        //if this assert is hit than that ill need to write a bit more code to account for that situation
        assert (SceneVulkanParameters::numBytesPerColor % storageBufferMemRequirements.alignment == 0);
    }
#endif


    // Allocate device local memory that will hold the storage buffer data to be accessed from the gpu
    VkDeviceMemory storageBufMem = AllocateVkBufferMemory (physicalDevice, logicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &storageBufferMemRequirements);

    //bind the storage buffer memory
    vkBindBufferMemory (logicalDevice, storageBufferHandle, storageBufMem, 0);

    return {/*...VkBuffer.......bufferHandle......*/ storageBufferHandle,
            /*...VkDeviceMemory.memoryHandle......*/ storageBufMem,
            /*...VkDeviceSize...buffersize........*/ storageBufferMemRequirements.size,
            /*...uint32_t.......offset............*/ 0 };
}

inline vulkanAllocatedBufferInfo CreateAndAllocateUniformBuffer (VkPhysicalDevice physicalDevice,
                                                                 VkDevice         logicalDevice,
                                                                 uint32_t         bufferSizeInBytes,
                                                                 uint32_t         queueIndex)
{
    //@TODO: change the naming here so its not matrix buffer
    VkBuffer           sceneTransformUBO               = VK_NULL_HANDLE;
    VkBufferUsageFlags sceneTransformBufferUsageFlags  = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VkBufferCreateInfo sceneTransformxBufferCreateInfo =
    {
        /*...VkStructureType........sType.....................*/ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        /*...const.void*............pNext.....................*/ 0, //reminder: buffer device address struct would need to go here if using that feature
        /*...VkBufferCreateFlags....flags.....................*/ 0,
        /*...VkDeviceSize...........size......................*/ bufferSizeInBytes,
        /*...VkBufferUsageFlags.....usage.....................*/ sceneTransformBufferUsageFlags,
        /*...VkSharingMode..........sharingMode...............*/ VK_SHARING_MODE_EXCLUSIVE,
        /*...uint32_t...............queueFamilyIndexCount.....*/ 1,
        /*...const.uint32_t*........pQueueFamilyIndices.......*/ &queueIndex
    };

    VkResult             result                      = vkCreateBuffer (logicalDevice, &sceneTransformxBufferCreateInfo, 0, &sceneTransformUBO);
    VkMemoryRequirements matrixBufferMemRequirements = {};
    vkGetBufferMemoryRequirements (logicalDevice, sceneTransformUBO, &matrixBufferMemRequirements);

    // Allocate device local memory that will hold the vertex data to be accessed from the gpu
    VkDeviceMemory matrixMem = AllocateVkBufferMemory (physicalDevice,
                                                       logicalDevice,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                       &matrixBufferMemRequirements);

    //bind the vertex buffer memory
    vkBindBufferMemory (logicalDevice, sceneTransformUBO, matrixMem, 0);

    return {/*...VkBuffer.......bufferHandle......*/ sceneTransformUBO,
            /*...VkDeviceMemory.memoryHandle......*/ matrixMem,
            /*...VkDeviceSize...buffersize........*/ matrixBufferMemRequirements.size,
            /*...uint32_t.......offset............*/ 0 };
}

void ExecuteBuffer2BufferCopy (VkPhysicalDevice          physicalDevice,
                               VkDevice                  logicalDevice,
                               VkQueue                   queue,
                               uint32_t                  queueFamilyIndex,
                               VkDeviceSize              copySize,
                               vulkanAllocatedBufferInfo srcBufferInfo,
                               vulkanAllocatedBufferInfo dstBufferInfo)
{
    // Now we need to create and execute a command buffer that will call vkCmdCopyBuffer to copy the vertex data from the staging buffer to the vertex buffer
    VkCommandPoolCreateInfo commandPoolCreateInfo =
    {
        /*..VkStructureType.............sType.................*/ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        /*..const.void*.................pNext.................*/ 0,
        /*..VkCommandPoolCreateFlags....flags.................*/ VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        /*..uint32_t....................queueFamilyIndex......*/ queueFamilyIndex
    };

    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkResult      result      = vkCreateCommandPool (logicalDevice, &commandPoolCreateInfo, 0, &commandPool);
    assert (result == VK_SUCCESS);

    VkCommandBuffer             bufferCopyCmdBuffer       = VK_NULL_HANDLE;
    VkCommandBufferAllocateInfo commandBufferAllocateInfo =
    {
        /*..VkStructureType.........sType................*/ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        /*..const.void*.............pNext................*/ 0,
        /*..VkCommandPool...........commandPool..........*/ commandPool,
        /*..VkCommandBufferLevel....level................*/ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        /*..uint32_t................commandBufferCount...*/ 1
    };

    result = vkAllocateCommandBuffers (/*...VkDevice...........................device...........*/ logicalDevice,
                                       /*...const.VkCommandBufferAllocateInfo*.pAllocateInfo....*/ &commandBufferAllocateInfo,
                                       /*...VkCommandBuffer*...................pCommandBuffers..*/ &bufferCopyCmdBuffer);

    VkCommandBufferBeginInfo cmdBuffBeginInfo =
    {
        /*..VkStructureType..........................sType................*/ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        /*..const.void*..............................pNext................*/ 0,
        /*..VkCommandBufferUsageFlags................flags................*/ VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        /*..const.VkCommandBufferInheritanceInfo*....pInheritanceInfo.....*/ 0
    };

    // Record the copy command
    vkBeginCommandBuffer (bufferCopyCmdBuffer, &cmdBuffBeginInfo);
    {
        VkBufferCopy copyInfo =
        {
            /*...VkDeviceSize....srcOffset...*/ srcBufferInfo.offset,
            /*...VkDeviceSize....dstOffset...*/ dstBufferInfo.offset,
            /*...VkDeviceSize....size........*/ copySize
        };

        vkCmdCopyBuffer (/*...VkCommandBuffer.............................commandBuffer...*/ bufferCopyCmdBuffer,
                         /*...VkBuffer....................................srcBuffer.......*/ srcBufferInfo.bufferHandle,
                         /*...VkBuffer....................................dstBuffer.......*/ dstBufferInfo.bufferHandle,
                         /*...uint32_t....................................regionCount.....*/ 1,
                         /*...const.VkBufferCopy*.........................pRegions........*/ &copyInfo);
    }
    vkEndCommandBuffer (bufferCopyCmdBuffer);

    VkSubmitInfo submitInfo =
    {
        /*...VkStructureType................sType...................*/ VK_STRUCTURE_TYPE_SUBMIT_INFO,
        /*...const.void*....................pNext...................*/ nullptr,
        /*...uint32_t.......................waitSemaphoreCount......*/ 0,
        /*...const.VkSemaphore*.............pWaitSemaphores.........*/ nullptr,
        /*...const.VkPipelineStageFlags*....pWaitDstStageMask.......*/ nullptr,
        /*...uint32_t.......................commandBufferCount......*/ 1,
        /*...const.VkCommandBuffer*.........pCommandBuffers.........*/ &bufferCopyCmdBuffer,
        /*...uint32_t.......................signalSemaphoreCount....*/ 0,
        /*...const.VkSemaphore*.............pSignalSemaphores.......*/ nullptr
    };

    vkQueueSubmit (queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle (queue);

    vkFreeCommandBuffers (logicalDevice, commandPool, 1, &bufferCopyCmdBuffer);
}

vulkanAllocatedBufferInfo CreateAndAllocaStagingBuffer (VkPhysicalDevice physicalDevice,
                                                        VkDevice         logicalDevice,
                                                        uint32_t         bufferSizeInBytes,
                                                        uint32_t         queueIndex)
{
    // Create staging buffer first
    VkBuffer           stagingBufferHandle     = VK_NULL_HANDLE;
    VkBufferCreateInfo stagingBufferCreateInfo =
    {
        /*...VkStructureType........sType.....................*/ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        /*...const.void*............pNext.....................*/ 0, //reminder: buffer device address struct would need to go here if using that feature
        /*...VkBufferCreateFlags....flags.....................*/ 0,
        /*...VkDeviceSize...........size......................*/ bufferSizeInBytes,
        /*...VkBufferUsageFlags.....usage.....................*/ VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        /*...VkSharingMode..........sharingMode...............*/ VK_SHARING_MODE_EXCLUSIVE,
        /*...uint32_t...............queueFamilyIndexCount.....*/ 1,
        /*...const.uint32_t*........pQueueFamilyIndices.......*/ &queueIndex
    };

    // Create the staging buffer
    VkResult result = vkCreateBuffer (logicalDevice,
                                      &stagingBufferCreateInfo,
                                      nullptr,
                                      &stagingBufferHandle);

    // Get the memory requirements and check that they meet our needs
    VkMemoryRequirements stagingBufferMemRequirements = {};
    vkGetBufferMemoryRequirements (logicalDevice, stagingBufferHandle, &stagingBufferMemRequirements);
    assert (bufferSizeInBytes <= stagingBufferMemRequirements.size);

    // Allocate memory for the buffer.
    VkDeviceMemory stagingMem = AllocateVkBufferMemory (physicalDevice,
                                                        logicalDevice,
                                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                                                        &stagingBufferMemRequirements);
    assert (stagingMem != VK_NULL_HANDLE);

    // Bind the allocated memory to the buffer
    vkBindBufferMemory (logicalDevice, stagingBufferHandle, stagingMem, 0);

    return {/*...VkBuffer.......bufferHandle......*/ stagingBufferHandle,
            /*...VkDeviceMemory.memoryHandle......*/ stagingMem,
            /*...VkDeviceSize...buffersize........*/ stagingBufferMemRequirements.size,
            /*...uint32_t.......offset............*/ 0 };
}

GeometryBufferSet CreateGeometryBuffersAndAABBs (VkPhysicalDevice       physicalDevice,
                                                    VkDevice            logicalDevice,
                                                    VkQueue             queue,
                                                    uint32_t            queueFamilyIndex,
                                                    const aiScene*      pScene) // ex: const aiScene* pScene = aiImportFile (pFilePath, aiProcessPreset_TargetRealtime_MaxQuality);
{
    assert(physicalDevice      != VK_NULL_HANDLE);
    assert(logicalDevice       != VK_NULL_HANDLE);
    assert(queueFamilyIndex    != UINT32_MAX    );
    assert(pScene              != nullptr       );
    assert(pScene->HasMeshes() == true          );


    static const glm::mat4 sIdentityMat4x4 = glm::mat4 (1.0f, 0.0f, 0.0f, 0.0f,
                                                        0.0f, 1.0f, 0.0f, 0.0f,
                                                        0.0f, 0.0f, 1.0f, 0.0f,
                                                        0.0f, 0.0f, 0.0f, 1.0f);
    const uint32_t numMeshes = pScene->mNumMeshes;

    // Initialize scene aabb with min/max coordinates derived from the first vertex in the first mesh
    //    Cant initialzie fields with 0, because 0 isnt guarenteed to be inside a meshs actual AABB and could incorrectly set the min or max for some or all axis.
    VkAabbPositionsKHR sceneAABB =
    {
        /*...float....minX...*/ pScene->mMeshes[0]->mVertices[0].x,
        /*...float....minY...*/ pScene->mMeshes[0]->mVertices[0].y,
        /*...float....minZ...*/ pScene->mMeshes[0]->mVertices[0].z,
        /*...float....maxX...*/ pScene->mMeshes[0]->mVertices[0].x,
        /*...float....maxY...*/ pScene->mMeshes[0]->mVertices[0].y,
        /*...float....maxZ...*/ pScene->mMeshes[0]->mVertices[0].z
    };

    vulkanAllocatedBufferInfo vertexStagingBufferInfo  = {}; // Will correspond to a buffer backed by host visible memory, and will be where we initally write vertex data
    vulkanAllocatedBufferInfo indexStagingBufferInfo   = {}; // Will correspond to a buffer backed by host visible memory, and will be where vert indices defining primitives will be written
    vulkanAllocatedBufferInfo vertexBufferInfo         = {};
    vulkanAllocatedBufferInfo indexBufferInfo          = {};
    VkDeviceSize              singleVertexSize         = 0; // Size of all the vertex attributes
    VkDeviceSize              vertexBufferDataSize     = 0;
    VkDeviceSize              indexBufferDataSize      = 0;
    uint32_t                  sceneTriangleCount       = 0;
    uint32_t                  sceneVertexCount         = 0;

    MeshInfo*  pMeshInfos = static_cast<MeshInfo*>(calloc(numMeshes, sizeof(MeshInfo)));

    // Calculate number of verticies and primitives present in all the meshes in the scene.
    for (uint32_t meshIdx = 0; meshIdx < numMeshes; meshIdx++)
    {
        const aiMesh* pAiMesh = pScene->mMeshes[meshIdx];

        sceneVertexCount   += pAiMesh->mNumVertices;
        sceneTriangleCount += pAiMesh->mNumFaces;
    }
    
    singleVertexSize = SceneVulkanParameters::numBytesPerVertexPosition + SceneVulkanParameters::numBytesPerVertexNormal;

    // Calculate amount of memory needed to hold the vertex data
    vertexBufferDataSize  = sceneVertexCount * singleVertexSize;

    // Calculated amount of memory needed to hold the index buffer data
    indexBufferDataSize   = sceneTriangleCount * SceneVulkanParameters::numIndexBytesPerPrimitive;

    // Create a staging buffer which will be where the cpu writes vertex data to
    vertexStagingBufferInfo = CreateAndAllocaStagingBuffer (physicalDevice,
                                                            logicalDevice,
                                                            vertexBufferDataSize,
                                                            queueFamilyIndex);

    // Create a staging buffer which will be where the cpu writes index data to
    indexStagingBufferInfo = CreateAndAllocaStagingBuffer (physicalDevice,
                                                           logicalDevice,
                                                           indexBufferDataSize,
                                                           queueFamilyIndex);

    // Map vertex buffer and index buffer memory , than get typed pointers to it
    void*     pMappedPositionStagingBufferMem = MapBufferMemory (vertexStagingBufferInfo, logicalDevice);
    void*     pMappedIndexStagingBufferMem    = MapBufferMemory (indexStagingBufferInfo,  logicalDevice);

    VertexAttributeData* pVertexBuffMemPtr    = reinterpret_cast<VertexAttributeData*>(pMappedPositionStagingBufferMem);
    uint32_t*            pIndexBuffMemUintPtr = reinterpret_cast< uint32_t* >(pMappedIndexStagingBufferMem);

    // Reset and re-use counters
    sceneTriangleCount = 0;
    sceneVertexCount   = 0;

    uint32_t currentCoordIdx = 0;
    // This loop accomplishes 4 things:
    // - Write vertex position data to the vertex staging buffer
    // - Initialize MeshInfo structs
    // - Write index buffer data to index staging buffer
    // - Update scene aabb on a per mesh basis after calculating a mesh's own aabb
    for (uint32_t meshIdx = 0; meshIdx < numMeshes; meshIdx++)
    {
        const aiMesh*      pAiMesh            = pScene->mMeshes[meshIdx];
        uint32_t           firstPrimInMesh    = sceneTriangleCount;     // save off index of the first triangle that is part of this mesh
        uint32_t           firstVertexForMesh = sceneVertexCount;

        assert (pAiMesh->HasPositions() == true);
        assert (pAiMesh->HasNormals()   == true);
        // Initialize mesh's AABB with coordinates from first vertex.
        //    Cant initialzie fields with 0, because 0 isnt guarenteed to be inside meshs actual AABB.
        VkAabbPositionsKHR meshAABB =
        {
            /*...float....minX...*/  pAiMesh->mVertices[0].x,
            /*...float....minY...*/  pAiMesh->mVertices[0].y,
            /*...float....minZ...*/  pAiMesh->mVertices[0].z,
            /*...float....maxX...*/  pAiMesh->mVertices[0].x,
            /*...float....maxY...*/  pAiMesh->mVertices[0].y,
            /*...float....maxZ...*/  pAiMesh->mVertices[0].z
        };


        printf ("----------===== Vertex Data for mesh %u  =====------\n", meshIdx);
        // Write vertex position data to the vertex position staging buffer, and update the mesh AABB.
        for (uint32_t meshVertexIdx = 0; meshVertexIdx < pAiMesh->mNumVertices; meshVertexIdx++)
        {
            const aiVector3D* pVertex         = &(pAiMesh->mVertices[meshVertexIdx]);
            const aiVector3D* pNormal         = &(pAiMesh->mNormals[meshVertexIdx]);
            const uint32_t    vertexIdx       = sceneVertexCount + meshVertexIdx;

            VertexAttributeData* pMeshVertex = &(pVertexBuffMemPtr[vertexIdx]);

            // Position attribute
             pMeshVertex->position[0] = pVertex->x;
             pMeshVertex->position[1] = pVertex->y;
             pMeshVertex->position[2] = pVertex->z;

            // Normal attribute
             pMeshVertex->normal[0] = pNormal->x;
             pMeshVertex->normal[1] = pNormal->y;
             pMeshVertex->normal[2] = pNormal->z;

            // Update mesh aabb as needed
            if (pVertex->x > meshAABB.maxX) { meshAABB.maxX  = pVertex->x; } // update x max
            if (pVertex->x < meshAABB.minX) { meshAABB.minX  = pVertex->x; } // update x min
            if (pVertex->y > meshAABB.maxY) { meshAABB.maxY  = pVertex->y; } // update y max
            if (pVertex->y < meshAABB.minY) { meshAABB.minY  = pVertex->y; } // update y min
            if (pVertex->z > meshAABB.maxZ) { meshAABB.maxZ  = pVertex->z; } // update z max
            if (pVertex->z < meshAABB.minZ) { meshAABB.minZ  = pVertex->z; } // update z min
        }

        sceneVertexCount += pAiMesh->mNumVertices;

        printf ("----------===== Triangle Index Data for mesh %u  =====------\n", meshIdx);
        // Write vertex index data to the vertex index staging buffer
        for (uint32_t meshPrimIndex = 0; meshPrimIndex < pAiMesh->mNumFaces; meshPrimIndex++)
        {
            const aiFace* pFace = &(pAiMesh->mFaces[meshPrimIndex]);

            if (pFace->mNumIndices == SceneVulkanParameters::verticiesPerPrimitive)
            {
                // offset of the buffer element that holds the vertex index which willserve  as the provoking vertex of this triangle
                const uint32_t index0WriteLoc = (firstPrimInMesh + meshPrimIndex) * SceneVulkanParameters::verticiesPerPrimitive;
                const aiFace*  pFace          = &(pAiMesh->mFaces[meshPrimIndex]);

                // Im assuming these indices are stored with a consistent winding order. May need to verify later
                pIndexBuffMemUintPtr[index0WriteLoc    ] = firstVertexForMesh + pFace->mIndices[2];
                pIndexBuffMemUintPtr[index0WriteLoc + 1] = firstVertexForMesh + pFace->mIndices[1];
                pIndexBuffMemUintPtr[index0WriteLoc + 2] = firstVertexForMesh + pFace->mIndices[0];
            }
            else if (pFace->mNumIndices < 3) {    printf ("Warning: found degenerate primitive!\n"); } // degenerate prim
            else if (pFace->mNumIndices > 3) {    printf("Warning: found Ngon!\n");                  } // an Ngon
        } 

        sceneTriangleCount += pAiMesh->mNumFaces;

        // Update meshInfo struct
        pMeshInfos[meshIdx] = { /*...uint32_t.............firstPrimIdx...*/ firstPrimInMesh,
                                /*...uint32_t.............numPrims.......*/ sceneTriangleCount - firstPrimInMesh,
                                /*...uint32_t.............materialIdx....*/ pAiMesh->mMaterialIndex,
                                /*...glm::mat4x4..........modelMatrix....*/ sIdentityMat4x4,
                                /*...VkAabbPositionsKHR...aabb...........*/ meshAABB };

        // Updating AABB for the whole scene based on mesh AABBs
        if (meshAABB.maxX > sceneAABB.maxX) { sceneAABB.maxX = meshAABB.maxX; } // update x max
        if (meshAABB.minX < sceneAABB.minX) { sceneAABB.minX = meshAABB.minX; } // update x min
        if (meshAABB.maxY > sceneAABB.maxY) { sceneAABB.maxY = meshAABB.maxY; } // update y max
        if (meshAABB.minY < sceneAABB.minY) { sceneAABB.minY = meshAABB.minY; } // update y min
        if (meshAABB.maxZ > sceneAABB.maxZ) { sceneAABB.maxZ = meshAABB.maxZ; } // update z max
        if (meshAABB.minZ < sceneAABB.minZ) { sceneAABB.minZ = meshAABB.minZ; } // update z min

    } // END: for (uint32_t meshIdx = 0; meshIdx < pScene->mNumMeshes; meshIdx++)

    vkUnmapMemory (logicalDevice, vertexStagingBufferInfo.memoryHandle);
    vkUnmapMemory (logicalDevice, indexStagingBufferInfo.memoryHandle);

    // Create and allocate memory for a device local vertex buffer
    vertexBufferInfo = CreateAndAllocateVertexBuffer (physicalDevice,
                                                      logicalDevice,
                                                      vertexBufferDataSize,
                                                      queueFamilyIndex);

    // Create and allocate memory for a device local index buffer
    indexBufferInfo  = CreateAndAllocateIndexBuffer (physicalDevice,
                                                     logicalDevice,
                                                     indexBufferDataSize,
                                                     queueFamilyIndex);


    // Sanity checking the size of what we've created so far
    assert (indexBufferDataSize  <= indexStagingBufferInfo.buffersize   );
    assert (indexBufferDataSize  <= indexBufferInfo.buffersize          );
    assert (vertexBufferDataSize <= vertexStagingBufferInfo.buffersize  );
    assert (vertexBufferDataSize <= vertexBufferInfo.buffersize         );

    //@TODO: create a function to record the buffer2buffer copy for the index and vertex buffer in the same command buffer
    //               To be even more efficient i should also be able to create a single staging buffer that would be the size of the vertex AND position data. 
    //                   Than the relevant data would be copied into separate device visible buffers at their individal offsets inside the staging buffer.
    //@TODO: Also check if the device local vertex and index buffers can be backed by memory from the same allocation. 
    //             I doubt index and vertex buffers need to use different memory types, so it should be fine. but double check
            
    // Upload vertex buffer data to the device local buffer
    ExecuteBuffer2BufferCopy (/*...VkPhysicalDevice..........physicalDevice.....*/ physicalDevice,
                              /*...VkDevice..................logicalDevice......*/ logicalDevice,
                              /*...VkQueue...................queue..............*/ queue,
                              /*...uint32_t..................queueFamilyIndex...*/ queueFamilyIndex,
                              /*...VkDeviceSize..............copySize...........*/ vertexBufferDataSize,
                              /*...vulkanAllocatedBufferInfo.srcBufferInfo......*/ vertexStagingBufferInfo,  // Src buffer
                              /*...vulkanAllocatedBufferInfo.dstBufferInfo......*/ vertexBufferInfo);        // Dst buffer

    // Upload index buffer data to the device local buffer
    ExecuteBuffer2BufferCopy (/*...VkPhysicalDevice..........physicalDevice.......*/ physicalDevice,
                                /*...VkDevice..................logicalDevice......*/ logicalDevice,
                                /*...VkQueue...................queue..............*/ queue,
                                /*...uint32_t..................queueFamilyIndex...*/ queueFamilyIndex,
                                /*...VkDeviceSize..............copySize...........*/ indexBufferDataSize,
                                /*...vulkanAllocatedBufferInfo.srcBufferInfo......*/ indexStagingBufferInfo, // Src buffer
                                /*...vulkanAllocatedBufferInfo.dstBufferInfo......*/ indexBufferInfo);       // Dst buffer

    // We dont need the staging buffers anymore since the data is now in the buffers backed by device local memory
    //     so we destroy the staging buffers and free their memory.
    vkFreeMemory (logicalDevice, vertexStagingBufferInfo.memoryHandle, nullptr);
    vkDestroyBuffer (logicalDevice, vertexStagingBufferInfo.bufferHandle, nullptr);

    vkFreeMemory (logicalDevice, indexStagingBufferInfo.memoryHandle, nullptr);
    vkDestroyBuffer (logicalDevice, indexStagingBufferInfo.bufferHandle, nullptr);

    // returns a GeometryBufferSet struct
    return { /*...vulkanAllocatedBufferInfo.vertexBufferInfo...*/ vertexBufferInfo,
             /*...vulkanAllocatedBufferInfo.indexBufferInfo....*/ indexBufferInfo,
             /*...uint32_t..................numVertices........*/ sceneVertexCount,
             /*...uint32_t..................numTriangles.......*/ sceneTriangleCount,
             /*...uint32_t..................numMeshes..........*/ numMeshes,
             /*...const..MeshInfo*..........pMeshes............*/ pMeshInfos,
             /*...VkAabbPositionsKHR........sceneAABB..........*/ sceneAABB  };
}

vulkanAllocatedBufferInfo CreateMeshColorsStorageBuffer (VkPhysicalDevice    physicalDevice,
                                                         VkDevice            logicalDevice,
                                                         VkQueue             queue,
                                                         uint32_t            queueFamilyIndex,
                                                         const aiScene*      pScene)
{

    uint32_t numMaterialColors = pScene->HasMaterials () ? pScene->mNumMaterials : 0;
    assert (numMaterialColors > 0);

    // Initialize with size required for the storing the per material values. At this point thats just one color per material.
    VkDeviceSize storageBufferDataSize = numMaterialColors * SceneVulkanParameters::numBytesPerColor;

    //Create a staging buffer which will that the cpu writes color data to
    vulkanAllocatedBufferInfo storageStagingBufferInfo = CreateAndAllocaStagingBuffer (physicalDevice,
                                                                                       logicalDevice,
                                                                                       storageBufferDataSize,
                                                                                       queueFamilyIndex);

    //Map the buffer memory and than cast it to a float4 for convenience.
    glm::vec4* pStorageStagingMemFloat4Ptr = reinterpret_cast<glm::vec4*>(MapBufferMemory (storageStagingBufferInfo, logicalDevice));

#ifdef DEBUG
    // Make sure color channel data is packed
    static_assert(offsetof (glm::vec4, r) == 0);
    static_assert(offsetof (glm::vec4, g) == 4);
    static_assert(offsetof (glm::vec4, b) == 8);
    static_assert(offsetof (glm::vec4, a) == 12);
#endif

    //@TODO: Account for different alignments: Subsequent colors must be at addresses that are >= the alignment reported from the memRequirements.
    printf ("\n============== Material Colors ==============\n");
    printf ("numMaterials = %u\n", pScene->mNumMaterials);
    for (uint32_t materialIdx = 0; materialIdx < pScene->mNumMaterials; materialIdx++)
    {
        // Get the material's diffuse color using assimp's material interface
        aiMaterial* pMaterial = pScene->mMaterials[materialIdx];
        aiColor3D   color     = {};
        pMaterial->Get (AI_MATKEY_COLOR_DIFFUSE, color);

        //
        const glm::vec3 colorInLinearSpace = { color.r, color.g, color.b };
        glm::vec3       colorInSrgbSpace   = glm::convertLinearToSRGB (colorInLinearSpace);

        //Write the color to the correct location in the staging buffer
        pStorageStagingMemFloat4Ptr[materialIdx] = { /*.r.*/ colorInSrgbSpace.r,
                                                     /*.g.*/ colorInSrgbSpace.g,
                                                     /*.b.*/ colorInSrgbSpace.b,
                                                     /*.a.*/ 1.0f               };

        printf ("pScene->mMaterials[%u].get(AI_MATKEY_COLOR_DIFFUSE) = srgb - { %.4f, %.4f, %.4f }\n", materialIdx,
                                                                                                       colorInSrgbSpace.r,
                                                                                                       colorInSrgbSpace.g,
                                                                                                       colorInSrgbSpace.b);
    }
    printf ("\n\n");

    // Unmap staging buffer
    vkUnmapMemory (logicalDevice, storageStagingBufferInfo.memoryHandle);


    vulkanAllocatedBufferInfo colorStorageBufferInfo = CreateAndAllocateSsbo (physicalDevice,
                                                                              logicalDevice,
                                                                              storageBufferDataSize,
                                                                              queueFamilyIndex);

    // Upload transform uniform data to device local buffer
    ExecuteBuffer2BufferCopy (/*...VkPhysicalDevice............physicalDevice........*/ physicalDevice,
                              /*...VkDevice....................logicalDevice.........*/ logicalDevice,
                              /*...VkQueue.....................queue.................*/ queue,
                              /*...uint32_t....................queueFamilyIndex......*/ queueFamilyIndex,
                              /*...VkDeviceSize................copySize..............*/ storageBufferDataSize,
                              /*...vulkanAllocatedBufferInfo...srcBufferInfo.........*/ storageStagingBufferInfo,   // Src buffer
                              /*...vulkanAllocatedBufferInfo...dstBufferInfo.........*/ colorStorageBufferInfo);    // Dst buffer

    // Dont need the staging buffer or memory anymore
    vkFreeMemory (logicalDevice, storageStagingBufferInfo.memoryHandle, nullptr);
    vkDestroyBuffer (logicalDevice, storageStagingBufferInfo.bufferHandle, nullptr);

    return colorStorageBufferInfo;
}

vulkanAllocatedBufferInfo CreateUniformBuffer (VkPhysicalDevice             physicalDevice,
                                               VkDevice                     logicalDevice,
                                               VkQueue                      queue,
                                               uint32_t                     queueFamilyIndex,
                                               const GeometryBufferSet*     pGeometryBufferSet,
                                               VkAabbPositionsKHR*          pDesiredSceneBounds,
                                               bool                         maintainAspectRatio)
{
    // Defining a struct here that matches the UBO data layout described in the shader.
    struct UniformBufferData
    {
        // Scene Transform
        glm::mat4 sceneTransform;
        glm::vec4 sceneScale;

        // Scene ambient color
        glm::vec4  ambient_color;

        // Light location
        glm::vec4 lightLocation;
        glm::vec4 lightIntensities;
    };

#ifdef DEBUG
    // Make sure the struct were using is packed
    const uint32_t numvec4s = 8;
    static_assert (sizeof (UniformBufferData) == numvec4s * 4 * sizeof (float));

#endif

    // Initialize with size required for the storing the scene transform
    VkDeviceSize uniformBufferDataSize = sizeof (UniformBufferData);

    //Create a staging buffer which will be where the cpu writes matrix data to
    vulkanAllocatedBufferInfo uniformStagingBufferInfo = CreateAndAllocaStagingBuffer (physicalDevice,
                                                                                       logicalDevice,
                                                                                       uniformBufferDataSize,
                                                                                       queueFamilyIndex);

    // Initializing sceneTransformUBO to a transform that will translate and scale the scene such that if fits inside the AABB defined by *pDesiredSceneBounds
    glm::mat4x4 sceneTransform = GetTransform_FitAABBToAABB (/*...VkAabbPositionsKHR...originalAABB...............*/ pGeometryBufferSet->sceneAABB,
                                                             /*...VkAabbPositionsKHR...desiredBounds..............*/ *pDesiredSceneBounds,
                                                             /*...bool.................maintainSceneAspectRatio...*/ maintainAspectRatio);
    PrintNamedMatrix ("sceneTransform", &sceneTransform);

    void*              pMappedUniformStagingBufferMem = MapBufferMemory (uniformStagingBufferInfo, logicalDevice);
    UniformBufferData* pUniformStagingBuffer          = reinterpret_cast<UniformBufferData*>(pMappedUniformStagingBufferMem);

    float ambientCoeficient = 0.15f;

    // Write UBO data
    *pUniformStagingBuffer =
    {
        /*...mat4...sceneTransform.........*/ sceneTransform,
        /*...vec4...sceneScale.............*/ glm::vec4 (1.0f,   1.0f,  1.0f, 1.0f),
        /*...vec3...ambient_color..........*/ glm::vec4 (0.4f,   0.4f,  0.4f, 1.0f) * ambientCoeficient,
        /*...vec3...lightLocation..........*/ glm::vec4 (-0.25f, -1.0f,  2.0f, 1.0f),
        /*...vec3...lightIntensities.......*/ glm::vec4 (1.0f,   1.0f,  1.0f, 1.0f)
    };

    // Unmap staging buffer
    vkUnmapMemory (logicalDevice, uniformStagingBufferInfo.memoryHandle);

    // Create and allocate memory for a device local buffer that will contain the UBO data
    vulkanAllocatedBufferInfo uniformBufferInfo = CreateAndAllocateUniformBuffer (physicalDevice, // Create and allocate memory for a device local UBO with per-mesh data
                                                                                  logicalDevice,
                                                                                  uniformBufferDataSize,
                                                                                  queueFamilyIndex);

    // Upload transform uniform data to device local buffer
    ExecuteBuffer2BufferCopy (/*...VkPhysicalDevice..........physicalDevice.....*/ physicalDevice,
                              /*...VkDevice..................logicalDevice......*/ logicalDevice,
                              /*...VkQueue...................queue..............*/ queue,
                              /*...uint32_t..................queueFamilyIndex...*/ queueFamilyIndex,
                              /*...VkDeviceSize..............copySize...........*/ uniformBufferDataSize,
                              /*...vulkanAllocatedBufferInfo.srcBufferInfo......*/ uniformStagingBufferInfo,   // Src buffer
                              /*...vulkanAllocatedBufferInfo.dstBufferInfo......*/ uniformBufferInfo);         // Dst buffer

    // Dont need the staging buffer or memory anymore
    vkFreeMemory    (logicalDevice, uniformStagingBufferInfo.memoryHandle, nullptr);
    vkDestroyBuffer (logicalDevice, uniformStagingBufferInfo.bufferHandle, nullptr);

    return uniformBufferInfo;
}

#endif // VULKAN_BUFFER_TOOLS_CPP