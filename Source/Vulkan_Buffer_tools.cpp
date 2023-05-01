
#include "Vulkan_Buffer_Tools.h"


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
        VkMemoryPropertyFlags flags                     = physicalDeviceMemoryProperties.memoryTypes[memTypeIdx].propertyFlags;
        uint32_t              supportedMemTypesBitMask = memRequirements->memoryTypeBits;

        bool isMemTypeSupportedForResource     = ((memRequirements->memoryTypeBits & (1 << memTypeIdx)) > 0) ? true : false;
        bool areRequiredMemPropertiesSupported = ((flags & requiredPropertyFlags) == requiredPropertyFlags ) ? true : false;

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
    assert (result    == VK_SUCCESS);
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
    // Create vertex buffer
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

VkResult ExecuteBuffer2BufferCopy (VkPhysicalDevice          physicalDevice,
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

    return VK_SUCCESS;// should add have some logic here
}

vulkanAllocatedBufferInfo CreateStagingBuffer (VkPhysicalDevice physicalDevice,
                                               VkDevice         logicalDevice,
                                               uint32_t         bufferSizeInBytes,
                                               uint32_t         queueIndex)
{
    // Create staging buffer first
    VkBuffer           stagingBuffer = VK_NULL_HANDLE;
    VkBufferUsageFlags stagingBufferUsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkBufferCreateInfo stagingBufferCreateInfo =
    {
        /*...VkStructureType........sType.....................*/ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        /*...const.void*............pNext.....................*/ 0, //reminder: buffer device address struct would need to go here if using that feature
        /*...VkBufferCreateFlags....flags.....................*/ 0,
        /*...VkDeviceSize...........size......................*/ bufferSizeInBytes,
        /*...VkBufferUsageFlags.....usage.....................*/ stagingBufferUsageFlags,
        /*...VkSharingMode..........sharingMode...............*/ VK_SHARING_MODE_EXCLUSIVE,
        /*...uint32_t...............queueFamilyIndexCount.....*/ 1,
        /*...const.uint32_t*........pQueueFamilyIndices.......*/ &queueIndex
    };

    VkResult result = vkCreateBuffer (logicalDevice, &stagingBufferCreateInfo, 0, &stagingBuffer);

    VkMemoryRequirements stagingBufferMemRequirements = {};
    vkGetBufferMemoryRequirements (logicalDevice, stagingBuffer, &stagingBufferMemRequirements);
    assert (bufferSizeInBytes <= stagingBufferMemRequirements.size);

    // Allocate memory for the buffer.
    VkDeviceMemory stagingMem = AllocateVkBufferMemory (physicalDevice, logicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &stagingBufferMemRequirements);
    assert (stagingMem != VK_NULL_HANDLE);

    // Bind the allocated memory to the buffer
    vkBindBufferMemory (logicalDevice, stagingBuffer, stagingMem, 0);

    return {/*...VkBuffer.......bufferHandle......*/ stagingBuffer,
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
    GeometryBufferSet         geometryBuffers         = {};
    vulkanAllocatedBufferInfo vertexStagingBufferInfo = {}; // Will correspond to a buffer backed by host visible memory, and will be where we initally write vertex data
    vulkanAllocatedBufferInfo indexStagingBufferInfo  = {}; // Will correspond to a buffer backed by host visible memory, and will be where vert indices defining primitives will be written
    VkDeviceSize              vertexBufferDataSize    = 0;
    VkDeviceSize              indexBufferDataSize     = 0;
    uint32_t                  numVertices             = 0;
    uint32_t                  numTriangles            = 0;

    if ((physicalDevice   != VK_NULL_HANDLE) &&
        (logicalDevice    != VK_NULL_HANDLE) &&
        (queueFamilyIndex != UINT32_MAX    ) &&
        (pScene           != nullptr       ))
    {

        if (pScene->HasMeshes () == true)
        {
            uint32_t numVertices  = 0;
            uint32_t numPrims     = 0;  // This number may account for degenerate triangles, or Ngons as well as triangles. The number of renderable triangles will be calculated below.

            //Get required buffer size
            for (uint32_t meshIdx = 0; meshIdx < pScene->mNumMeshes; meshIdx++)
            {
                const aiMesh* pAiMesh = pScene->mMeshes[meshIdx];

                numVertices  += pAiMesh->mNumVertices;
                numPrims += pAiMesh->mNumFaces;
            }

            // Calculate amount of memory needed to hold the vertex data
            vertexBufferDataSize = numVertices * NUM_BYTES_PER_VERTEX_POSITION;
            indexBufferDataSize  = numPrims * NUM_INDEX_BYTES_PER_TRIANGLE;

            // Create a staging buffer which will be where the cpu writes vertex data to
            vertexStagingBufferInfo = CreateStagingBuffer (physicalDevice,
                                                           logicalDevice,
                                                           vertexBufferDataSize,
                                                           queueFamilyIndex);

            // Create a staging buffer which will be where the cpu writes index data to
            indexStagingBufferInfo = CreateStagingBuffer (physicalDevice,
                                                          logicalDevice,
                                                          indexBufferDataSize,
                                                          queueFamilyIndex);
            // Map vertex buffer and index buffer memory
            void* pMappedVertexBufferMem = MapBufferMemory (vertexStagingBufferInfo, logicalDevice);
            void* pMappedIndexBufferMem  = MapBufferMemory (indexStagingBufferInfo,  logicalDevice);

            float*    pVertexBuffMemFloatPtr = reinterpret_cast<float*>(pMappedVertexBufferMem);
            uint32_t* pIndexBuffMemUintPtr   = reinterpret_cast<uint32_t*>(pMappedIndexBufferMem);
            uint32_t triangleWriteIndex = 0;
            uint32_t vertexWriteIndex   = 0;
            //Write vertex position data to the buffer
            for (uint32_t meshIdx = 0; meshIdx < pScene->mNumMeshes; meshIdx++)
            {
                const aiMesh* pAiMesh = pScene->mMeshes[meshIdx];

                for (uint32_t meshVertexIdx = 0; meshVertexIdx < pAiMesh->mNumVertices; meshVertexIdx++)
                {
                    const uint32_t xCoordWriteIdx = vertexWriteIndex * COORDS_PER_POSITION;
                    pVertexBuffMemFloatPtr[xCoordWriteIdx    ] = pAiMesh->mVertices->x;
                    pVertexBuffMemFloatPtr[xCoordWriteIdx + 1] = pAiMesh->mVertices->y;
                    pVertexBuffMemFloatPtr[xCoordWriteIdx + 2] = pAiMesh->mVertices->z;

                    vertexWriteIndex++;

                    // 
                    //          if (useInterleavedAttributes == true)
                    //          {
                    //              // Write normal data.
                    //              // Write color data.
                    //              // etc...
                    //          }
                }

               

                for (uint32_t meshPrimIndex = 0; meshPrimIndex < pAiMesh->mNumFaces; meshPrimIndex++)
                {
                    const aiFace* pFace = &(pAiMesh->mFaces[meshPrimIndex]);
                    assert (pFace);

                    if (pFace->mNumIndices == 3)
                    {
                        const uint32_t provokingVertexIndex = triangleWriteIndex * NUM_VERTICES_PER_TRIANGLE;

                        // Im assuming these indices are stored with a consistent winding order. May need to verify later
                        pIndexBuffMemUintPtr[provokingVertexIndex    ] = pAiMesh->mFaces[meshPrimIndex].mIndices[0];
                        pIndexBuffMemUintPtr[provokingVertexIndex + 1] = pAiMesh->mFaces[meshPrimIndex].mIndices[1];
                        pIndexBuffMemUintPtr[provokingVertexIndex + 2] = pAiMesh->mFaces[meshPrimIndex].mIndices[2];

                        triangleWriteIndex++;
                    }
                    //else if(pFace->mNumIndices < 3) // degenerate prim
                    //else if(pFace->mNumIndices > 3) // an Ngon

                }
            }

            geometryBuffers.numVertices  = vertexWriteIndex;
            geometryBuffers.numTriangles = triangleWriteIndex;

            vkUnmapMemory (logicalDevice, vertexStagingBufferInfo.memoryHandle);
            vkUnmapMemory (logicalDevice, indexStagingBufferInfo.memoryHandle);


            geometryBuffers.vertexBufferInfo = CreateAndAllocateVertexBuffer (physicalDevice,
                                                                              logicalDevice,
                                                                              vertexBufferDataSize,
                                                                              queueFamilyIndex);

            geometryBuffers.indexBufferInfo = CreateAndAllocateIndexBuffer (physicalDevice,
                                                                            logicalDevice,
                                                                            indexBufferDataSize,
                                                                            queueFamilyIndex);

            assert ( indexBufferDataSize  <= indexStagingBufferInfo.buffersize           );
            assert ( indexBufferDataSize  <= geometryBuffers.indexBufferInfo.buffersize  );
            assert ( vertexBufferDataSize <= vertexStagingBufferInfo.buffersize          );
            assert (vertexBufferDataSize  <= geometryBuffers.vertexBufferInfo.buffersize );

            //now upload data to the staging buffer.
                //@TODO: create a function to record the buffer2buffer copy for the index and vertex buffer in the same command buffer

            ExecuteBuffer2BufferCopy (physicalDevice,
                                      logicalDevice,
                                      queue,
                                      queueFamilyIndex,
                                      vertexBufferDataSize,
                                      vertexStagingBufferInfo,           // Src buffer
                                      geometryBuffers.vertexBufferInfo); // Dst buffer

            ExecuteBuffer2BufferCopy (physicalDevice,
                                      logicalDevice,
                                      queue,
                                      queueFamilyIndex,
                                      indexBufferDataSize,
                                      indexStagingBufferInfo, // Src buffer
                                      geometryBuffers.indexBufferInfo);       // Dst buffer

            // We dont need the staging buffers anymore since the data is now in the buffers backed by device local memory
            //     so we destroy the staging buffers and free their memory.
            vkFreeMemory    (logicalDevice, vertexStagingBufferInfo.memoryHandle, nullptr);
            vkDestroyBuffer (logicalDevice, vertexStagingBufferInfo.bufferHandle, nullptr);

            vkFreeMemory    (logicalDevice, indexStagingBufferInfo.memoryHandle, nullptr);
            vkDestroyBuffer (logicalDevice, indexStagingBufferInfo.bufferHandle, nullptr);

            vertexStagingBufferInfo = {};
            indexStagingBufferInfo  = {};
        }
    }
    else
    {
        printf ("Cant create buffer. invalid argument.  Invalid |%s|%s|%s|%s|\n", (physicalDevice   == VK_NULL_HANDLE) ? "VkPhysicalDevice" : "|",
                                                                                  (logicalDevice    == VK_NULL_HANDLE) ? "VkDevice"         : "|",
                                                                                  (queueFamilyIndex == UINT32_MAX    ) ? "queueIndex"       : "|",
                                                                                  (pScene           == nullptr       ) ? "const aiScene"    : "|");
        assert (0);
    }

    return geometryBuffers;
}
