
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

void CreateVertexBuffer (VkPhysicalDevice physicalDevice,
                         VkDevice         logicalDevice,
                         uint32_t         bufferSizeInBytes,
                         uint32_t         queueIndex,
                         VkQueue          queue,
                         void*            pVertexData, //@TODO: Split up this function such that creating and allocating the staging buffer is a separate function.
                         VkBuffer*        pVertexBufferOut,
                         VkDeviceMemory*  pVertexBufferMemoryOut)
{
    // Create staging buffer first
    VkBuffer           stagingBuffer           = VK_NULL_HANDLE;
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

    VkResult result = vkCreateBuffer (logicalDevice, &stagingBufferCreateInfo, 0, &stagingBuffer);

    VkMemoryRequirements stagingBufferMemRequirements = {};
    vkGetBufferMemoryRequirements (logicalDevice, stagingBuffer, &stagingBufferMemRequirements);
    assert (bufferSizeInBytes <= stagingBufferMemRequirements.size);

    //Allocate memory for the buffer.
    VkDeviceMemory stagingMem = AllocateVkBufferMemory (physicalDevice, logicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &stagingBufferMemRequirements);

    // Bind the allocated memory to the buffer
    vkBindBufferMemory (logicalDevice, stagingBuffer, stagingMem, 0);


    //Fill the staging buffer with vertex data, so it can be copied to a device local buffer after this
    void* pMappedStagingBuffer;
    vkMapMemory (logicalDevice, stagingMem, 0, bufferSizeInBytes, 0, &pMappedStagingBuffer);
    memcpy (pMappedStagingBuffer, pVertexData, bufferSizeInBytes);
    vkUnmapMemory (logicalDevice, stagingMem);

    // Create vertex buffer
    VkBuffer           vertexBuffer = VK_NULL_HANDLE;
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

    result = vkCreateBuffer (logicalDevice, &vertexBufferCreateInfo, 0, &vertexBuffer);

    VkMemoryRequirements vertexBufferMemRequirements = {};
    vkGetBufferMemoryRequirements (logicalDevice, vertexBuffer, &vertexBufferMemRequirements);

    // Allocate device local memory that will hold the vertex data to be accessed from the gpu
    VkDeviceMemory vertexMem = AllocateVkBufferMemory (physicalDevice, logicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vertexBufferMemRequirements);

    //bind the vertex buffer memory
    vkBindBufferMemory (logicalDevice, vertexBuffer, vertexMem, 0);

    // Now we need to create and execute a command buffer that will call vkCmdCopyBuffer to copy the vertex data from the staging buffer to the vertex buffer
    VkCommandPoolCreateInfo commandPoolCreateInfo =
    {
        /*..VkStructureType.............sType.................*/ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        /*..const.void*.................pNext.................*/ 0,
        /*..VkCommandPoolCreateFlags....flags.................*/ VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, //why not re-use command buffers? look into this later. @TODO
        /*..uint32_t....................queueFamilyIndex......*/ queueIndex
    };

    VkCommandPool commandPool = VK_NULL_HANDLE;

    result = vkCreateCommandPool (logicalDevice, &commandPoolCreateInfo, 0, &commandPool);
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

    result = vkAllocateCommandBuffers ( /*...VkDevice...........................device..........*/ logicalDevice,
                                       /*...const.VkCommandBufferAllocateInfo*.pAllocateInfo....*/ &commandBufferAllocateInfo,
                                       /*...VkCommandBuffer*...................pCommandBuffers..*/ &bufferCopyCmdBuffer);

    VkCommandBufferBeginInfo cmdBuffBeginInfo =
    {
        /*..VkStructureType..........................sType................*/ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        /*..const.void*..............................pNext................*/ 0,
        /*..VkCommandBufferUsageFlags................flags................*/ VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        /*..const.VkCommandBufferInheritanceInfo*....pInheritanceInfo.....*/ 0
    };

    vkBeginCommandBuffer (bufferCopyCmdBuffer, &cmdBuffBeginInfo);
    {
        VkBufferCopy copyInfo =
        {
            /*...VkDeviceSize....srcOffset...*/ 0,
            /*...VkDeviceSize....dstOffset...*/ 0,
            /*...VkDeviceSize....size........*/ bufferSizeInBytes
        };

        vkCmdCopyBuffer (bufferCopyCmdBuffer, stagingBuffer, vertexBuffer, 1, &copyInfo);
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

    // Now that the data has been copied to the device local buffer, we should delete the staging buffer
    vkFreeMemory (logicalDevice, stagingMem, 0);
    vkDestroyBuffer (logicalDevice, stagingBuffer, 0);

    vkFreeCommandBuffers (logicalDevice, commandPool, 1, &bufferCopyCmdBuffer);

    *pVertexBufferOut       = vertexBuffer;
    *pVertexBufferMemoryOut = vertexMem;
}
