
#include "Vulkan_Buffer_Tools.h"
#include "assimp/scene.h"

void GetMemoryType (VkPhysicalDevice      physicalDevice,
                    VkMemoryPropertyFlags requiredPropertyFlags, // ex: a mask of VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, etc...
                    uint32_t*             pChosenMemTypeIdxOut,
                    VkMemoryRequirements* memRequirements) //a bitmask and contains one bit set for every supported memory type for the resource. Bit i is set if and only if the memory type i in the VkPhysicalDeviceMemoryProperties structure for the physical device is supported for the resource.
{
    assert (requiredPropertyFlags != 0);
    VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = {};
    vkGetPhysicalDeviceMemoryProperties (physicalDevice, &physicalDeviceMemoryProperties);

    for (uint32_t memTypeIdx = 0; memTypeIdx < physicalDeviceMemoryProperties.memoryTypeCount; memTypeIdx++)
    {
        VkMemoryPropertyFlags flags = physicalDeviceMemoryProperties.memoryTypes[memTypeIdx].propertyFlags;
        uint32_t              supportedMemTypesBitMask = memRequirements->memoryTypeBits;

        bool isMemTypeSupportedForResource = ((memRequirements->memoryTypeBits & (1 << memTypeIdx)) > 0) ? true : false;
        bool areRequiredMemPropertiesSupported = ((flags & requiredPropertyFlags) == requiredPropertyFlags) ? true : false;

        if (isMemTypeSupportedForResource && areRequiredMemPropertiesSupported)
        {
            *pChosenMemTypeIdxOut = memTypeIdx;
            //  *pChosenHeapIndexOut = physicalDeviceMemoryProperties.memoryTypes[memTypeIdx].heapIndex;
        }
    }
}

VkDeviceMemory AllocateVkBufferMemory (VkPhysicalDevice      physicalDevice,
                                       VkDevice              logicalDevice,
                                       VkMemoryPropertyFlags requiredPropertyFlags,
                                       VkMemoryRequirements* pBufferMemoryRequirements)
{
    uint32_t memTypeIndex;

    GetMemoryType (physicalDevice, requiredPropertyFlags, &memTypeIndex, pBufferMemoryRequirements);
    VkMemoryAllocateInfo allocateInfo =
    {
        /*...VkStructureType....sType..............*/ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        /*...const.void*........pNext..............*/ 0,
        /*...VkDeviceSize.......allocationSize.....*/ pBufferMemoryRequirements->size,
        /*...uint32_t...........memoryTypeIndex....*/ memTypeIndex
    };

    VkDeviceMemory deviceMem = VK_NULL_HANDLE;

    VkResult result = vkAllocateMemory (logicalDevice, &allocateInfo, nullptr, &deviceMem);
    assert (result == VK_SUCCESS);
    assert (deviceMem != VK_NULL_HANDLE);

    return deviceMem;
}


vulkanAllocatedBufferInfo CreateAndAllocateVertexBuffer (VkPhysicalDevice physicalDevice,
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
    VkDeviceMemory vertexMem = AllocateVkBufferMemory (physicalDevice, logicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBufferMemRequirements);

    //bind the vertex buffer memory
    vkBindBufferMemory (logicalDevice, vertexBuffer, vertexMem, 0);

    return {/*...VkBuffer.......bufferHandle......*/ vertexBuffer,
            /*...VkDeviceMemory.memoryHandle......*/ vertexMem,
            /*...VkDeviceSize...buffersize........*/ vertexBufferMemRequirements.size,
            /*...uint32_t.......offset............*/ 0 };
}

vulkanAllocatedBufferInfo CreateAndAllocateIndexBuffer (VkPhysicalDevice physicalDevice,
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

     //bind the vertex buffer memory
    vkBindBufferMemory (logicalDevice, indexBuffer, indexMem, 0);

    return {/*...VkBuffer.......bufferHandle......*/ indexBuffer,
            /*...VkDeviceMemory.memoryHandle......*/ indexMem,
            /*...VkDeviceSize...buffersize........*/ indexBufferMemRequirements.size,
            /*...uint32_t.......offset............*/ 0 };
}

///@TODO: Make a more general buffer create function
vulkanAllocatedBufferInfo CreateAndAllocateMatrixBuffer (VkPhysicalDevice physicalDevice,
                                                         VkDevice         logicalDevice,
                                                         uint32_t         bufferSizeInBytes,
                                                         uint32_t         queueIndex)
{
    VkBuffer           matrixBuffer           = VK_NULL_HANDLE;
    VkBufferUsageFlags matrixBufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    VkBufferCreateInfo matrixBufferCreateInfo =
    {
        /*...VkStructureType........sType.....................*/ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        /*...const.void*............pNext.....................*/ 0, //reminder: buffer device address struct would need to go here if using that feature
        /*...VkBufferCreateFlags....flags.....................*/ 0,
        /*...VkDeviceSize...........size......................*/ bufferSizeInBytes,
        /*...VkBufferUsageFlags.....usage.....................*/ matrixBufferUsageFlags,
        /*...VkSharingMode..........sharingMode...............*/ VK_SHARING_MODE_EXCLUSIVE,
        /*...uint32_t...............queueFamilyIndexCount.....*/ 1,
        /*...const.uint32_t*........pQueueFamilyIndices.......*/ &queueIndex
    };

    VkResult result = vkCreateBuffer (logicalDevice, &matrixBufferCreateInfo, 0, &matrixBuffer);

    VkMemoryRequirements matrixBufferMemRequirements = {};
    vkGetBufferMemoryRequirements (logicalDevice, matrixBuffer, &matrixBufferMemRequirements);

    // Allocate device local memory that will hold the vertex data to be accessed from the gpu
    VkDeviceMemory matrixMem = AllocateVkBufferMemory (physicalDevice, logicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &matrixBufferMemRequirements);

    //bind the vertex buffer memory
    vkBindBufferMemory (logicalDevice, matrixBuffer, matrixMem, 0);

    return {/*...VkBuffer.......bufferHandle......*/ matrixBuffer,
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
        /*..VkCommandPoolCreateFlags....flags.................*/ VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, //why not re-use command buffers? look into this later. @TODO
        /*..uint32_t....................queueFamilyIndex......*/ queueFamilyIndex
    };

    VkCommandPool commandPool = VK_NULL_HANDLE;

    VkResult result = vkCreateCommandPool (logicalDevice, &commandPoolCreateInfo, 0, &commandPool);
    assert (result == VK_SUCCESS);

    VkCommandBuffer             bufferCopyCmdBuffer = VK_NULL_HANDLE;
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

    VkResult result = vkCreateBuffer (logicalDevice, &stagingBufferCreateInfo, nullptr, &stagingBufferHandle);

    VkMemoryRequirements stagingBufferMemRequirements = {};
    vkGetBufferMemoryRequirements (logicalDevice, stagingBufferHandle, &stagingBufferMemRequirements);
    assert (bufferSizeInBytes <= stagingBufferMemRequirements.size);

    // Allocate memory for the buffer.
    VkDeviceMemory stagingMem = AllocateVkBufferMemory (physicalDevice, logicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &stagingBufferMemRequirements);
    assert (stagingMem != VK_NULL_HANDLE);

    // Bind the allocated memory to the buffer
    vkBindBufferMemory (logicalDevice, stagingBufferHandle, stagingMem, 0);

    return {/*...VkBuffer.......bufferHandle......*/ stagingBufferHandle,
            /*...VkDeviceMemory.memoryHandle......*/ stagingMem,
            /*...VkDeviceSize...buffersize........*/ stagingBufferMemRequirements.size,
            /*...uint32_t.......offset............*/ 0 };
}

GeometryBufferSet CreateGeometryBuffersFromAiScene (VkPhysicalDevice physicalDevice, // @TODO, Create ssbo (or ubo maybe) with individual transform matrices for files which define multiple geometries/objects/meshes/whatever you want to call them
                                                    VkDevice         logicalDevice,
                                                    VkQueue          queue,
                                                    uint32_t         queueFamilyIndex,
                                                    const aiScene*   pScene, // ex: const aiScene* pScene = aiImportFile (pFilePath, aiProcessPreset_TargetRealtime_MaxQuality);
                                                    bool             useInterleavedAttributes)
{
    GeometryBufferSet         geometryBuffersOut      = {};
    vulkanAllocatedBufferInfo vertexStagingBufferInfo = {}; // Will correspond to a buffer backed by host visible memory, and will be where we initally write vertex data
    vulkanAllocatedBufferInfo indexStagingBufferInfo  = {}; // Will correspond to a buffer backed by host visible memory, and will be where vert indices defining primitives will be written
    vulkanAllocatedBufferInfo matrixStagingBufferInfo = {};
    VkDeviceSize              vertexBufferDataSize    = 0;
    VkDeviceSize              indexBufferDataSize     = 0;
    VkDeviceSize              matrixBufferDataSize    = 0;
    uint32_t                  numVertices             = 0;
    uint32_t                  numTriangles            = 0;
    uint32_t                  numMatrices             = 0;

    if ((physicalDevice   != VK_NULL_HANDLE) &&
        (logicalDevice    != VK_NULL_HANDLE) &&
        (queueFamilyIndex != UINT32_MAX    ) &&
        (pScene           != nullptr       ))
    {
        if (pScene->HasMeshes () == true)
        {
            numMatrices                  = pScene->mNumMeshes;
            geometryBuffersOut.numMeshes = numMatrices;
            geometryBuffersOut.pMeshes   = static_cast<MeshInfo*>(calloc(numMatrices, sizeof(MeshInfo)));

            // Calculate number of verticies and primitives present in all the meshes in the scene.
            for (uint32_t meshIdx = 0; meshIdx < pScene->mNumMeshes; meshIdx++)
            {
                const aiMesh* pAiMesh = pScene->mMeshes[meshIdx];

                numVertices  += pAiMesh->mNumVertices;
                numTriangles += pAiMesh->mNumFaces;
                ///@note: This would be a good place to record the offsets of each mesh in the vertex buffer if that becomes relevant.
            }

            // Calculate amount of memory needed to hold the vertex data
            vertexBufferDataSize = numVertices  * NUM_BYTES_PER_VERTEX_POSITION;
            indexBufferDataSize  = numTriangles * NUM_INDEX_BYTES_PER_TRIANGLE;
            matrixBufferDataSize = numMatrices  * NUM_BYTES_PER_MODEL_MATRIX;

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

            // Create a staging buffer which will be where the cpu writes matrix data to
            matrixStagingBufferInfo = CreateAndAllocaStagingBuffer (physicalDevice,
                                                                    logicalDevice,
                                                                    matrixBufferDataSize,
                                                                    queueFamilyIndex);

            // Map vertex buffer and index buffer memory 
            void* pMappedPositionStagingBufferMem = MapBufferMemory (vertexStagingBufferInfo, logicalDevice);
            void* pMappedIndexStagingBufferMem    = MapBufferMemory (indexStagingBufferInfo,  logicalDevice);
            void* pMappedMatrixStagingBufferMem   = MapBufferMemory (matrixStagingBufferInfo, logicalDevice);

            float*    pVertexBuffMemFloatPtr = reinterpret_cast<  float*   >(pMappedPositionStagingBufferMem);
            uint32_t* pIndexBuffMemUintPtr   = reinterpret_cast< uint32_t* >(pMappedIndexStagingBufferMem);
            float*    pModelMatrixFloatPtr   = reinterpret_cast<  float*   >(pMappedMatrixStagingBufferMem);

            uint32_t  numSceneTriangles      = 0;
            uint32_t  numSceneVertices       = 0;

            //Write vertex position data to the buffer
            for (uint32_t meshIdx = 0; meshIdx < pScene->mNumMeshes; meshIdx++)
            {
                const aiMesh* pAiMesh = pScene->mMeshes[meshIdx];

                // index of the first triangle that is part of this mesh
                uint32_t           firstPrimInMesh = numSceneTriangles;
                VkAabbPositionsKHR meshAABB        = {};
                glm::mat4x4        modelMatrix     = glm::mat4x4 (1.0, 0.0, 0.0, 0.0,
                                                                  0.0, 1.0, 0.0, 0.0,
                                                                  0.0, 0.0, 1.0, 0.0,
                                                                  0.0, 0.0, 0.0, 1.0);

                //@TODO: Check if the vertex data for a given mesh is stored contiguously,
                //                and if it is replace the loop below with a memcpy

                // Write vertex position data to the vertex position staging buffer.
                for (uint32_t meshVertexIdx = 0; meshVertexIdx < pAiMesh->mNumVertices; meshVertexIdx++)
                {
                    const aiVector3D* pVertex   = &(pAiMesh->mVertices[meshVertexIdx]);
                    const uint32_t    xCoordIdx = (numSceneVertices + meshVertexIdx)* COORDS_PER_POSITION;
                    pVertexBuffMemFloatPtr[xCoordIdx + 0] = pVertex->x;
                    pVertexBuffMemFloatPtr[xCoordIdx + 1] = pVertex->y;
                    pVertexBuffMemFloatPtr[xCoordIdx + 2] = pVertex->z;

                    /*
                    //          if (useInterleavedAttributes == true)
                    //          {
                    //              // Write normal data.
                    //              // Write color data.
                    //              // etc...
                    //         }*/

                    // Update mesh aabb as needed
                    if (pVertex->x > meshAABB.maxX) { meshAABB.maxX  = pVertex->x; } // update x max
                    if (pVertex->x < meshAABB.minX) { meshAABB.minX  = pVertex->x; } // update x min
                    if (pVertex->y > meshAABB.maxY) { meshAABB.maxY  = pVertex->y; } // update y max
                    if (pVertex->y < meshAABB.minY) { meshAABB.minY  = pVertex->y; } // update y min
                    if (pVertex->z > meshAABB.maxZ) { meshAABB.maxZ  = pVertex->z; } // update z max
                    if (pVertex->z < meshAABB.minZ) { meshAABB.minZ  = pVertex->z; } // update z min
                }

                numSceneVertices += pAiMesh->mNumVertices;

                
                // Write vertex index data to the  vertex index staging buffer
                for (uint32_t meshPrimIndex = 0; meshPrimIndex < pAiMesh->mNumFaces; meshPrimIndex++)
                {
                    const aiFace* pFace = &(pAiMesh->mFaces[meshPrimIndex]);

                    if (pFace->mNumIndices == 3)
                    {
                        // offset of the buffer element that holds the vertex index which willserve  as the provoking vertex of this triangle
                        const uint32_t provokingVertexIndexIdx = numSceneTriangles * NUM_VERTICES_PER_TRIANGLE;

                        // Im assuming these indices are stored with a consistent winding order. May need to verify later
                        pIndexBuffMemUintPtr[provokingVertexIndexIdx    ] = pAiMesh->mFaces[meshPrimIndex].mIndices[0];
                        pIndexBuffMemUintPtr[provokingVertexIndexIdx + 1] = pAiMesh->mFaces[meshPrimIndex].mIndices[1];
                        pIndexBuffMemUintPtr[provokingVertexIndexIdx + 2] = pAiMesh->mFaces[meshPrimIndex].mIndices[2];

                        numSceneTriangles++;
                    }
                    else if (pFace->mNumIndices < 3) {    printf ("Warning: found degenerate primitive!\n"); } // degenerate prim
                    else if (pFace->mNumIndices > 3) {    printf("Warning: found Ngon!\n");                  } // an Ngon

                }
                
                // Write the matrix data to the staging buffer
                memcpy (&(pModelMatrixFloatPtr[NUM_FLOATS_PER_TRASNFORM_MATRIX * meshIdx]), &(modelMatrix[0][0]), NUM_BYTES_PER_MODEL_MATRIX);

                geometryBuffersOut.pMeshes[meshIdx] = { /*...uint32_t...........firstPrimIdx...*/ firstPrimInMesh,
                                                        /*...uint32_t...........numPrims.......*/ numSceneTriangles - firstPrimInMesh,
                                                        /*...glm::mat4x4........modelMatrix....*/ modelMatrix,
                                                        /*...VkAabbPositionsKHR.aabb...........*/ meshAABB };

                // Updating AABB for the whole scene based on the mesh AABB
                if (meshAABB.maxX > geometryBuffersOut.aabb.maxX) { geometryBuffersOut.aabb.maxX = meshAABB.maxX; } // update x max
                if (meshAABB.minX < geometryBuffersOut.aabb.minX) { geometryBuffersOut.aabb.minX = meshAABB.minX; } // update x min
                if (meshAABB.maxY > geometryBuffersOut.aabb.maxY) { geometryBuffersOut.aabb.maxY = meshAABB.maxY; } // update y max
                if (meshAABB.minY < geometryBuffersOut.aabb.minY) { geometryBuffersOut.aabb.minY = meshAABB.minY; } // update y min
                if (meshAABB.maxZ > geometryBuffersOut.aabb.maxZ) { geometryBuffersOut.aabb.maxZ = meshAABB.maxZ; } // update z max
                if (meshAABB.minZ < geometryBuffersOut.aabb.minZ) { geometryBuffersOut.aabb.minZ = meshAABB.minZ; } // update z min
            }

            vkUnmapMemory (logicalDevice, vertexStagingBufferInfo.memoryHandle);
            vkUnmapMemory (logicalDevice, indexStagingBufferInfo.memoryHandle);

            geometryBuffersOut.numVertices      = numSceneVertices;
            geometryBuffersOut.numTriangles     = numSceneTriangles;
            geometryBuffersOut.vertexBufferInfo = CreateAndAllocateVertexBuffer (physicalDevice,   // Create and allocate memory for a device local vertex buffer
                                                                                 logicalDevice,
                                                                                 vertexBufferDataSize,
                                                                                 queueFamilyIndex);

            geometryBuffersOut.indexBufferInfo  = CreateAndAllocateIndexBuffer (physicalDevice,    // Create and allocate memory for a device local index buffer
                                                                               logicalDevice,
                                                                               indexBufferDataSize,
                                                                               queueFamilyIndex);

            geometryBuffersOut.matrixBufferInfo = CreateAndAllocateMatrixBuffer (physicalDevice, // Create and allocate memory for a device local UBO with per-mesh data
                                                                                 logicalDevice,
                                                                                 matrixBufferDataSize,
                                                                                 queueFamilyIndex);

            assert (indexBufferDataSize  <= indexStagingBufferInfo.buffersize            );
            assert (indexBufferDataSize  <= geometryBuffersOut.indexBufferInfo.buffersize);
            assert (vertexBufferDataSize <= vertexStagingBufferInfo.buffersize           );
            assert (vertexBufferDataSize <= geometryBuffersOut.vertexBufferInfo.buffersize);

            //now upload data to the staging buffer.
                //@TODO: create a function to record the buffer2buffer copy for the index and vertex buffer in the same command buffer
                //               To be even more efficient i should also be able to create a single staging buffer that would be the size of the vertex AND position data. 
                //                   Than the relevant data would be copied into separate device visible buffers at their individal offsets inside the staging buffer.
                //@TODO: Also check if the device local vertex and index buffers can be backed by memory from the same allocation. 
                //             I doubt index and vertex buffers need to use different memory types, so it should be fine. but double check
 
            ExecuteBuffer2BufferCopy (/*...VkPhysicalDevice..........physicalDevice........*/ physicalDevice,
                                      /*...VkDevice..................logicalDevice.........*/ logicalDevice,
                                      /*...VkQueue...................queue.................*/ queue,
                                      /*...uint32_t..................queueFamilyIndex......*/ queueFamilyIndex,
                                      /*...VkDeviceSize..............copySize..............*/ vertexBufferDataSize,
                                      /*...vulkanAllocatedBufferInfo.srcBufferInfo.........*/ vertexStagingBufferInfo,              // Src buffer
                                      /*...vulkanAllocatedBufferInfo.dstBufferInfo.........*/ geometryBuffersOut.vertexBufferInfo); // Dst buffer

            ExecuteBuffer2BufferCopy (/*...VkPhysicalDevice..........physicalDevice........*/ physicalDevice,
                                      /*...VkDevice..................logicalDevice.........*/ logicalDevice,
                                      /*...VkQueue...................queue.................*/ queue,
                                      /*...uint32_t..................queueFamilyIndex......*/ queueFamilyIndex,
                                      /*...VkDeviceSize..............copySize..............*/ indexBufferDataSize,
                                      /*...vulkanAllocatedBufferInfo.srcBufferInfo.........*/ indexStagingBufferInfo,               // Src buffer
                                      /*...vulkanAllocatedBufferInfo.dstBufferInfo.........*/ geometryBuffersOut.indexBufferInfo);  // Dst buffer

            ExecuteBuffer2BufferCopy (/*...VkPhysicalDevice..........physicalDevice........*/ physicalDevice,
                                      /*...VkDevice..................logicalDevice.........*/ logicalDevice,
                                      /*...VkQueue...................queue.................*/ queue,
                                      /*...uint32_t..................queueFamilyIndex......*/ queueFamilyIndex,
                                      /*...VkDeviceSize..............copySize..............*/ matrixBufferDataSize,
                                      /*...vulkanAllocatedBufferInfo.srcBufferInfo.........*/ matrixStagingBufferInfo,               // Src buffer
                                      /*...vulkanAllocatedBufferInfo.dstBufferInfo.........*/ geometryBuffersOut.matrixBufferInfo);  // Dst buffer

            // We dont need the staging buffers anymore since the data is now in the buffers backed by device local memory
            //     so we destroy the staging buffers and free their memory.
            vkFreeMemory (logicalDevice, vertexStagingBufferInfo.memoryHandle, nullptr);
            vkDestroyBuffer (logicalDevice, vertexStagingBufferInfo.bufferHandle, nullptr);

            vkFreeMemory (logicalDevice, indexStagingBufferInfo.memoryHandle, nullptr);
            vkDestroyBuffer (logicalDevice, indexStagingBufferInfo.bufferHandle, nullptr);

            vertexStagingBufferInfo = {};
            indexStagingBufferInfo = {};
        }
    }
    else
    {
        printf ("Cant create buffer. invalid argument.  Invalid |%s|%s|%s|%s|\n", (physicalDevice == VK_NULL_HANDLE) ? "VkPhysicalDevice" : "|",
                (logicalDevice == VK_NULL_HANDLE) ? "VkDevice" : "|",
                (queueFamilyIndex == UINT32_MAX) ? "queueIndex" : "|",
                (pScene == nullptr) ? "const aiScene" : "|");
        assert (0);
    }

    return geometryBuffersOut;
}