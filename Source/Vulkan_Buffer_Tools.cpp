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
                                                                VkDeviceSize     bufferSizeInBytes,
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

inline vulkanAllocatedBufferInfo CreateAndAllocateIndexBuffer (VkPhysicalDevice physicalDevice,
                                                               VkDevice         logicalDevice,
                                                               VkDeviceSize     bufferSizeInBytes,
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
                                                        VkDeviceSize     bufferSizeInBytes,
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
                                                                 VkDeviceSize     bufferSizeInBytes,
                                                                 uint32_t         queueIndex)
{
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
                               uint32_t                  graphicsQueueIndex,
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
        /*..uint32_t....................graphicsQueueIndex....*/ graphicsQueueIndex
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
                                                        VkDeviceSize     bufferSizeInBytes,
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

GeometryBufferSet CreateGeometryBuffersAndAABBs (VkPhysicalDevice    physicalDevice,
                                                 VkDevice            logicalDevice,
                                                 VkQueue             queue,
                                                 uint32_t            graphicsQueueIndex,
                                                 const aiScene*      pScene) // ex: const aiScene* pScene = aiImportFile (pFilePath, aiProcessPreset_TargetRealtime_MaxQuality);
{
    assert(physicalDevice      != VK_NULL_HANDLE);
    assert(logicalDevice       != VK_NULL_HANDLE);
    assert(graphicsQueueIndex  != UINT32_MAX    );
    assert(pScene              != nullptr       );
    assert(pScene->HasMeshes() == true          );

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
    vulkanAllocatedBufferInfo vertexBufferInfo         = {}; // Will correspond to a buffer backed by gpu local memory, and will be the buffer from which the vertex shader reads vertex data
    vulkanAllocatedBufferInfo indexBufferInfo          = {}; // Will correspond to a buffer backed by gpu local memory, and will contain index data used to determine which vertices form primitives.
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
                                                            graphicsQueueIndex);

    // Create a staging buffer which will be where the cpu writes index data to
    indexStagingBufferInfo = CreateAndAllocaStagingBuffer (physicalDevice,
                                                           logicalDevice,
                                                           indexBufferDataSize,
                                                           graphicsQueueIndex);

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
           // else if (pFace->mNumIndices < 3) {    printf ("Warning: found degenerate primitive!\n"); } // degenerate prim
           // else if (pFace->mNumIndices > 3) {    printf("Warning: found Ngon!\n");                  } // an Ngon
        } 

        sceneTriangleCount += pAiMesh->mNumFaces;

        // Update meshInfo struct
        pMeshInfos[meshIdx] = { /*...uint32_t.............firstPrimIdx...*/ firstPrimInMesh,
                                /*...uint32_t.............numPrims.......*/ sceneTriangleCount - firstPrimInMesh,
                                /*...uint32_t.............materialIdx....*/ pAiMesh->mMaterialIndex,
                                /*...glm::mat4x4..........modelMatrix....*/ glm::identity<glm::mat4>(),
                                /*...VkAabbPositionsKHR...aabb...........*/ meshAABB };

        // Updating AABB for the whole scene based on mesh AABBs
        if (meshAABB.maxX > sceneAABB.maxX) { sceneAABB.maxX = meshAABB.maxX; } // update x max
        if (meshAABB.minX < sceneAABB.minX) { sceneAABB.minX = meshAABB.minX; } // update x min
        if (meshAABB.maxY > sceneAABB.maxY) { sceneAABB.maxY = meshAABB.maxY; } // update y max
        if (meshAABB.minY < sceneAABB.minY) { sceneAABB.minY = meshAABB.minY; } // update y min
        if (meshAABB.maxZ > sceneAABB.maxZ) { sceneAABB.maxZ = meshAABB.maxZ; } // update z max
        if (meshAABB.minZ < sceneAABB.minZ) { sceneAABB.minZ = meshAABB.minZ; } // update z min

    } // END: for (uint32_t meshIdx = 0; meshIdx < pScene->mNumMeshes; meshIdx++)

    // Unmap staging buffer mem since were done writing to it
    vkUnmapMemory (logicalDevice, vertexStagingBufferInfo.memoryHandle);
    vkUnmapMemory (logicalDevice, indexStagingBufferInfo.memoryHandle);

    // Create and allocate memory for a device local vertex buffer
    vertexBufferInfo = CreateAndAllocateVertexBuffer (physicalDevice,
                                                      logicalDevice,
                                                      vertexBufferDataSize,
                                                      graphicsQueueIndex);

    // Create and allocate memory for a device local index buffer
    indexBufferInfo  = CreateAndAllocateIndexBuffer (physicalDevice,
                                                     logicalDevice,
                                                     indexBufferDataSize,
                                                     graphicsQueueIndex);


    // Sanity checking the size of what we've created so far
    assert (indexBufferDataSize  <= indexStagingBufferInfo.buffersize   );
    assert (indexBufferDataSize  <= indexBufferInfo.buffersize          );
    assert (vertexBufferDataSize <= vertexStagingBufferInfo.buffersize  );
    assert (vertexBufferDataSize <= vertexBufferInfo.buffersize         );

    //@TODO: create a function to record the buffer2buffer copy for the index and vertex buffer in the same command buffer
    //               To be even more efficient i should also be able to create a single staging buffer that would be the size of the vertex position data AND the index buffer data. 
    //                   Than the relevant data would be copied into separate device visible buffers at their individal offsets inside the staging buffer.
    //@TODO: Also check if the device local vertex and index buffers can be backed by memory from the same allocation. 
    //             I doubt index and vertex buffers need to use different memory types, so it should be fine. but double check.
            
    // Upload vertex buffer data to the device local buffer
    ExecuteBuffer2BufferCopy (/*...VkPhysicalDevice..........physicalDevice.....*/ physicalDevice,
                              /*...VkDevice..................logicalDevice......*/ logicalDevice,
                              /*...VkQueue...................queue..............*/ queue,
                              /*...uint32_t..................graphicsQueueIndex.*/ graphicsQueueIndex,
                              /*...VkDeviceSize..............copySize...........*/ vertexBufferDataSize,
                              /*...vulkanAllocatedBufferInfo.srcBufferInfo......*/ vertexStagingBufferInfo,  // Src buffer
                              /*...vulkanAllocatedBufferInfo.dstBufferInfo......*/ vertexBufferInfo);        // Dst buffer

    // Upload index buffer data to the device local buffer
    ExecuteBuffer2BufferCopy (/*...VkPhysicalDevice..........physicalDevice.......*/ physicalDevice,
                                /*...VkDevice..................logicalDevice......*/ logicalDevice,
                                /*...VkQueue...................queue..............*/ queue,
                                /*...uint32_t..................graphicsQueueIndex.*/ graphicsQueueIndex,
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

    printf ("\n============== Material Colors ==============\n");
    printf ("numMaterials = %u\n", pScene->mNumMaterials);
#endif
    //@TODO: Account for different alignments: Subsequent colors must be at addresses that are >= the alignment reported from the memRequirements.
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

#ifdef DEBUG
        printf ("pScene->mMaterials[%u].get(AI_MATKEY_COLOR_DIFFUSE) = srgb - { %.4f, %.4f, %.4f }\n", materialIdx,
                                                                                                       colorInSrgbSpace.r,
                                                                                                       colorInSrgbSpace.g,
                                                                                                       colorInSrgbSpace.b);
        if (materialIdx+1 == pScene->mNumMaterials) { printf ("\n\n"); }
#endif
    }

    // Unmap staging buffer
    vkUnmapMemory (logicalDevice, storageStagingBufferInfo.memoryHandle);

    // Create the buffer that will hold per-material colors.
    vulkanAllocatedBufferInfo colorStorageBufferInfo = CreateAndAllocateSsbo (physicalDevice,
                                                                              logicalDevice,
                                                                              storageBufferDataSize,
                                                                              queueFamilyIndex);

    // Upload transform uniform data to device local buffer
    ExecuteBuffer2BufferCopy (/*...VkPhysicalDevice............physicalDevice........*/ physicalDevice,
                              /*...VkDevice....................logicalDevice.........*/ logicalDevice,
                              /*...VkQueue.....................queue.................*/ queue,
                              /*...uint32_t....................graphicsQueueIndex......*/ queueFamilyIndex,
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
                                               UniformBufferData*           pInitialUboData,
                                               bool                         maintainAspectRatio)
{
#ifdef DEBUG
    // Make sure the data in the struct is packed. Working in vec4s, since that should be a safe alignment that will work in shaders
    const uint32_t numvec4s = 16;
    static_assert (sizeof (UniformBufferData) == numvec4s * 4 * sizeof (float));
#endif

    // Initialize with size required for the storing the scene transform
    VkDeviceSize uniformBufferDataSize = sizeof (UniformBufferData);

    //Create a staging buffer which will be where the cpu writes matrix data to
    vulkanAllocatedBufferInfo uniformStagingBufferInfo = CreateAndAllocaStagingBuffer (physicalDevice,
                                                                                       logicalDevice,
                                                                                       uniformBufferDataSize,
                                                                                       queueFamilyIndex);

    // Map the staging buffer memory
    void*              pMappedUniformStagingBufferMem = MapBufferMemory (uniformStagingBufferInfo, logicalDevice);
    UniformBufferData* pUniformStagingBuffer          = reinterpret_cast<UniformBufferData*>(pMappedUniformStagingBufferMem);
    {
        // Write initial UBO data to the staging buffer
        *pUniformStagingBuffer = *pInitialUboData;
    }
    // Unmap staging buffer
    vkUnmapMemory (logicalDevice, uniformStagingBufferInfo.memoryHandle);

    // Create and allocate memory for a device local buffer that will contain the UBO data
    vulkanAllocatedBufferInfo uniformBufferInfo = CreateAndAllocateUniformBuffer (physicalDevice,
                                                                                  logicalDevice,
                                                                                  uniformBufferDataSize,
                                                                                  queueFamilyIndex);

    // Upload transform uniform data to device local buffer
    ExecuteBuffer2BufferCopy (/*..VkPhysicalDevice............physicalDevice.......*/ physicalDevice,
                              /*..VkDevice....................logicalDevice........*/ logicalDevice,
                              /*..VkQueue.....................queue................*/ queue,
                              /*..uint32_t....................graphicsQueueIndex...*/ queueFamilyIndex,
                              /*..VkDeviceSize................copySize.............*/ uniformBufferDataSize,
                              /*..vulkanAllocatedBufferInfo...srcBufferInfo........*/ uniformStagingBufferInfo,   // Src buffer
                              /*..vulkanAllocatedBufferInfo...dstBufferInfo........*/ uniformBufferInfo);         // Dst buffer

    // Dont need the staging buffer or memory anymore
    vkFreeMemory    (logicalDevice, uniformStagingBufferInfo.memoryHandle, nullptr);
    vkDestroyBuffer (logicalDevice, uniformStagingBufferInfo.bufferHandle, nullptr);

    return uniformBufferInfo;
}

#endif // VULKAN_BUFFER_TOOLS_CPP