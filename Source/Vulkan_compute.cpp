#ifndef VULKAN_COMPUTE_CPP
#define VULKAN_COMPUTE_CPP

#include "Vulkan_compute.h"
#include "Vulkan_Utils.h"

VkPipeline CreateComputePipeline (VkDevice               logicalDevice,
                                  const char*            pComputeShaderPath,
                                  VkPipelineLayout*      pPipelineOut,
                                  VkDescriptorSetLayout* pDescriptorSetLayoutOut)
{
    static const uint32_t uniformBufferBinding = 0; // ubo for storing input data
    static const uint32_t storageBufferBinding = 1; // ssbo where the compute shader will write results


    VkDescriptorSetLayoutBinding pDescriptorSetLayoutBindings[] =
    {
        {
            /*...uint32_t..............binding................*/ uniformBufferBinding, // 0
            /*...VkDescriptorType......descriptorType.........*/ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /*...uint32_t..............descriptorCount........*/ 1, // This would be greater than 1 if ubo was declared in the shader like }ubo[n]; where n>1
            /*...VkShaderStageFlags....stageFlags.............*/ VK_SHADER_STAGE_COMPUTE_BIT,
            /*...const.VkSampler*......pImmutableSamplers.....*/ nullptr
        },
        {
            /*...uint32_t..............binding................*/ storageBufferBinding, // 1
            /*...VkDescriptorType......descriptorType.........*/ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            /*...uint32_t..............descriptorCount........*/ 1,
            /*...VkShaderStageFlags....stageFlags.............*/ VK_SHADER_STAGE_COMPUTE_BIT,
            /*...const.VkSampler*......pImmutableSamplers.....*/ nullptr
        }
    };

    static const uint32_t numDescriptorSetLayoutBindings = sizeof (pDescriptorSetLayoutBindings) / sizeof (VkDescriptorSetLayoutBinding);

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo =
    {
        /*...VkStructureType........................sType..........*/ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        /*...const.void*............................pNext..........*/ nullptr,
        /*...VkDescriptorSetLayoutCreateFlags.......flags..........*/ 0,
        /*...uint32_t...............................bindingCount...*/ numDescriptorSetLayoutBindings,
        /*...const.VkDescriptorSetLayoutBinding*....pBindings......*/ pDescriptorSetLayoutBindings
    };

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkResult              result              = vkCreateDescriptorSetLayout (logicalDevice, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout);
    assert (result == VK_SUCCESS);
    *pDescriptorSetLayoutOut = descriptorSetLayout;

    VkPipelineLayout           pipelineLayout   = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo layoutCreateInfo =
    {
        /*...VkStructureType.................sType......................*/ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        /*...const.void*.....................pNext......................*/ nullptr,
        /*...VkPipelineLayoutCreateFlags.....flags......................*/ 0,
        /*...uint32_t........................setLayoutCount.............*/ 1,
        /*...const.VkDescriptorSetLayout*....pSetLayouts................*/ & descriptorSetLayout,
        /*...uint32_t........................pushConstantRangeCount.....*/ 0,
        /*...const.VkPushConstantRange*......pPushConstantRanges........*/ nullptr
    };

    result = vkCreatePipelineLayout (logicalDevice, &layoutCreateInfo, 0, &pipelineLayout);
    assert (result == VK_SUCCESS);
    *pPipelineOut = pipelineLayout;


    VkComputePipelineCreateInfo computePipelineCreateInfo =
    {
        /*...VkStructureType....................sType................*/ VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        /*...const.void*........................pNext................*/ nullptr,
        /*...VkPipelineCreateFlags..............flags................*/ 0,
        /*...VkPipelineShaderStageCreateInfo....stage................*/ {
            /*...VkStructureType.....................sType.................*/ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            /*...const.void*.........................pNext.................*/ nullptr,
            /*...VkPipelineShaderStageCreateFlags....flags.................*/ 0,
            /*...VkShaderStageFlagBits...............stage.................*/ VK_SHADER_STAGE_COMPUTE_BIT ,
            /*...VkShaderModule......................module................*/ CreateShaderModule (logicalDevice, pComputeShaderPath),
            /*...const.char*.........................pName.................*/ "main",
            /*...const.VkSpecializationInfo*.........pSpecializationInfo...*/ nullptr },
        /*...VkPipelineLayout...................layout...............*/ pipelineLayout,
        /*...VkPipeline.........................basePipelineHandle...*/ VK_NULL_HANDLE,
        /*...int32_t............................basePipelineIndex....*/ 0,
    };

    VkPipeline pipeline = VK_NULL_HANDLE;
    result = vkCreateComputePipelines ( /*...VkDevice....................................device............*/ logicalDevice,
                                       /*...VkPipelineCache.............................pipelineCache.....*/ VK_NULL_HANDLE,
                                       /*...uint32_t....................................createInfoCount...*/ 1,
                                       /*...const.VkComputePipelineCreateInfo*..........pCreateInfos......*/ &computePipelineCreateInfo,
                                       /*...const.VkAllocationCallbacks*................pAllocator........*/ nullptr,
                                       /*...VkPipeline*.................................pPipelines........*/ &pipeline);

    return pipeline;
}


vulkanAllocatedBufferInfo CreateAndAllocateDeviceLocalBuffer (VkPhysicalDevice   physicalDevice,
                                                              VkDevice           logicalDevice,
                                                              VkDeviceSize       bufferSizeInBytes,
                                                              VkBufferUsageFlags bufferUsage,
                                                              uint32_t           queueIndex)
{
    VkBuffer           bufferHandle     = VK_NULL_HANDLE;
    VkBufferCreateInfo bufferCreateInfo =
    {
        /*...VkStructureType........sType.....................*/ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        /*...const.void*............pNext.....................*/ 0, //reminder: buffer device address struct would need to go here if using that feature
        /*...VkBufferCreateFlags....flags.....................*/ 0,
        /*...VkDeviceSize...........size......................*/ bufferSizeInBytes,
        /*...VkBufferUsageFlags.....usage.....................*/ bufferUsage,
        /*...VkSharingMode..........sharingMode...............*/ VK_SHARING_MODE_EXCLUSIVE,
        /*...uint32_t...............queueFamilyIndexCount.....*/ 1,
        /*...const.uint32_t*........pQueueFamilyIndices.......*/ &queueIndex
    };

    VkResult             result                = vkCreateBuffer (logicalDevice, &bufferCreateInfo, 0, &bufferHandle);
    VkMemoryRequirements bufferMemRequirements = {};
    vkGetBufferMemoryRequirements (logicalDevice, bufferHandle, &bufferMemRequirements);

    VkDeviceMemory mem = AllocateVkBufferMemory (physicalDevice,
                                                 logicalDevice,
                                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                 &bufferMemRequirements);

    vkBindBufferMemory (logicalDevice, bufferHandle, mem, 0);

    return {/*...VkBuffer.......bufferHandle......*/ bufferHandle,
        /*...VkDeviceMemory.memoryHandle......*/ mem,
        /*...VkDeviceSize...buffersize........*/ bufferMemRequirements.size,
        /*...uint32_t.......offset............*/ 0 };
}

vulkanAllocatedBufferInfo CreateAndAllocaStagingBuffer (VkPhysicalDevice physicalDevice,
                                                        VkDevice         logicalDevice,
                                                        VkDeviceSize     bufferSizeInBytes,
                                                       uint32_t         queueIndex)
{
    // Create staging buffer first
    VkBuffer           stagingBufferHandle = VK_NULL_HANDLE;
    VkBufferCreateInfo stagingBufferCreateInfo =
    {
        /*...VkStructureType........sType.....................*/ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        /*...const.void*............pNext.....................*/ 0, //reminder: buffer device address struct would need to go here if using that feature
        /*...VkBufferCreateFlags....flags.....................*/ 0,
        /*...VkDeviceSize...........size......................*/ bufferSizeInBytes,
        /*...VkBufferUsageFlags.....usage.....................*/ VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT ,
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

void ExecuteBuffer2BufferCopy (VkPhysicalDevice          physicalDevice,
                               VkDevice                  logicalDevice,
                               VkQueue                   queue,
                               uint32_t                  graphicsQueueIndex,
                               VkDeviceSize              copySize,
                               vulkanAllocatedBufferInfo srcBufferInfo,
                               vulkanAllocatedBufferInfo dstBufferInfo)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo =
    {
        /*..VkStructureType.............sType.................*/ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        /*..const.void*.................pNext.................*/ 0,
        /*..VkCommandPoolCreateFlags....flags.................*/ VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        /*..uint32_t....................queueFamilyIndex......*/ graphicsQueueIndex
    };

    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkResult      result = vkCreateCommandPool (logicalDevice, &commandPoolCreateInfo, 0, &commandPool);
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


inline VkCommandBuffer CreateAndAllocateCommandBuffer (VkPhysicalDevice physicalDevice,
                                                       VkDevice         logicalDevice,
                                                       uint32_t         queueIndex,
                                                       VkQueue          queueHandle,
                                                       VkCommandPool*   pCommandPoolOut)
{
    VkCommandPoolCreateInfo commandPoolCreateInfo =
    {
        /*..VkStructureType.............sType.................*/ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        /*..const.void*.................pNext.................*/ 0,
        /*..VkCommandPoolCreateFlags....flags.................*/ VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, //why not re-use command buffers? look into this later. @TODO
        /*..uint32_t....................queueFamilyIndex......*/ queueIndex
    };

    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkResult      result      = vkCreateCommandPool (logicalDevice, &commandPoolCreateInfo, 0, &commandPool);
    assert (result == VK_SUCCESS);


    VkCommandBufferAllocateInfo commandBufferAllocateInfo =
    {
        /*..VkStructureType.........sType................*/ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        /*..const.void*.............pNext................*/ 0,
        /*..VkCommandPool...........commandPool..........*/ commandPool,
        /*..VkCommandBufferLevel....level................*/ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        /*..uint32_t................commandBufferCount...*/ 1
    };

    VkCommandBuffer commandBufferHandle = VK_NULL_HANDLE;
    // Allocate a command buffer so that it can be used later be used to present this image to screen.
    result = vkAllocateCommandBuffers ( /*...VkDevice...........................device...........*/ logicalDevice,
                                        /*...const.VkCommandBufferAllocateInfo*.pAllocateInfo....*/ &commandBufferAllocateInfo,
                                        /*...VkCommandBuffer*...................pCommandBuffers..*/ &commandBufferHandle);

    return commandBufferHandle;
}

void RecordDispatch (VkPhysicalDevice physicalDevice,
                     VkDevice         logicalDevice,
                     VkCommandBuffer  cmdBuffer,
                     VkPipeline       computePipeline,
                     VkPipelineLayout pipelineLayout,
                     VkDescriptorSet  descriptorSet,
                     uint32_t         numWorkgroupsDispatched)
{
    VkCommandBufferBeginInfo cmdBufferBeginInfo =
    {
        /*...VkStructureType..........................sType..............*/ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        /*...const.void*..............................pNext..............*/ nullptr,
        /*...VkCommandBufferUsageFlags................flags..............*/ 0,
        /*...const.VkCommandBufferInheritanceInfo*....pInheritanceInfo...*/ nullptr
    };

    vkBeginCommandBuffer (cmdBuffer, &cmdBufferBeginInfo);

    vkCmdBindPipeline (cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    vkCmdBindDescriptorSets (cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

    vkCmdDispatch (cmdBuffer, numWorkgroupsDispatched, 1, 1);

    vkEndCommandBuffer (cmdBuffer);
}

VkDescriptorPool CreateDescriptorPool (VkDevice logicalDevice)
{
    const uint32_t numDescriptorTypes = ComputeParameters::numDescriptorTypesUsed;
    const uint32_t numUboDescriptors  = ComputeParameters::numUBOs;
    const uint32_t numSsboDescriptors = ComputeParameters::numSSBOs;

    VkDescriptorPoolSize pDescriptorPoolSizes[] =
    {
        {
            /*...VkDescriptorType....type................*/ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /*...uint32_t............descriptorCount.....*/ numUboDescriptors
        },
        {
            /*...VkDescriptorType....type................*/ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            /*...uint32_t............descriptorCount.....*/ numSsboDescriptors
        },
    };

    assert (sizeof (pDescriptorPoolSizes) / sizeof (VkDescriptorPoolSize) == numDescriptorTypes);

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo =
    {
        /*...VkStructureType................sType...........*/ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        /*...const.void*....................pNext...........*/ nullptr,
        /*...VkDescriptorPoolCreateFlags....flags...........*/ 0,
        /*...uint32_t.......................maxSets.........*/ 1,
        /*...uint32_t.......................poolSizeCount...*/ numDescriptorTypes, // Using one VkDescriptorPoolSize struct per descriptor type
        /*...const.VkDescriptorPoolSize*....pPoolSizes......*/ &pDescriptorPoolSizes[0]
    };

    VkDescriptorPool descriptorPoolHandle = VK_NULL_HANDLE;
    VkResult         result               = vkCreateDescriptorPool (logicalDevice,
                                                                    &descriptorPoolCreateInfo,
                                                                    nullptr,
                                                                    &descriptorPoolHandle);

    return descriptorPoolHandle;
}

VkDescriptorSet AllocateAndWriteDescriptorSet (VkDevice               logicalDevice,
                                               VkDescriptorPool       descriptorPoolHandle,
                                               VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                               VkBuffer               uniformBufferHandle,
                                               VkBuffer               storageBufferHandle)
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo =
    {
        /*...VkStructureType.................sType................*/ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        /*...const.void*.....................pNext................*/ nullptr,
        /*...VkDescriptorPool................descriptorPool.......*/ descriptorPoolHandle,
        /*...uint32_t........................descriptorSetCount...*/ ComputeParameters::numDescriptorSets,
        /*...const.VkDescriptorSetLayout*....pSetLayouts..........*/ &descriptorSetLayoutHandle
    };

    VkDescriptorSet descriptorSetHandle = VK_NULL_HANDLE;
    VkResult        result              = vkAllocateDescriptorSets (logicalDevice,
                                                                    &descriptorSetAllocateInfo,
                                                                    &descriptorSetHandle);
    assert (result == VK_SUCCESS);

    VkDescriptorBufferInfo uniformBufferDescriptorInfo =
    {
        /*...VkBuffer........buffer...*/ uniformBufferHandle,
        /*...VkDeviceSize....offset...*/ 0,
        /*...VkDeviceSize....range....*/ VK_WHOLE_SIZE
    };

    VkDescriptorBufferInfo storageBufferDescriptorInfo =
    {
        /*...VkBuffer........buffer...*/ storageBufferHandle,
        /*...VkDeviceSize....offset...*/ 0,
        /*...VkDeviceSize....range....*/ VK_WHOLE_SIZE
    };

    // Write the descriptor set with info about the resources backing the ubo and ssbo
    VkWriteDescriptorSet pDescriptorUpdateWrites[] =
    {
        { // struct describing the descriptor update for the uniform buffer at binding 0
            /*...VkStructureType..................sType..............*/ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            /*...const.void*......................pNext..............*/ nullptr,
            /*...VkDescriptorSet..................dstSet.............*/ descriptorSetHandle,
            /*...uint32_t.........................dstBinding.........*/ 0, // ubo binding in descriptor layout
            /*...uint32_t.........................dstArrayElement....*/ 0,
            /*...uint32_t.........................descriptorCount....*/ 1,
            /*...VkDescriptorType.................descriptorType.....*/ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /*...const.VkDescriptorImageInfo*.....pImageInfo.........*/ nullptr,
            /*...const.VkDescriptorBufferInfo*....pBufferInfo........*/ &uniformBufferDescriptorInfo,
            /*...const.VkBufferView*..............pTexelBufferView...*/ nullptr
        },
        { // struct describing the descriptor update for the storage buffer at binding 1
            /*...VkStructureType..................sType..............*/ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            /*...const.void*......................pNext..............*/ nullptr,
            /*...VkDescriptorSet..................dstSet.............*/ descriptorSetHandle,
            /*...uint32_t.........................dstBinding.........*/ 1, // ssbo binding in descriptor layout
            /*...uint32_t.........................dstArrayElement....*/ 0,
            /*...uint32_t.........................descriptorCount....*/ 1,
            /*...VkDescriptorType.................descriptorType.....*/ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            /*...const.VkDescriptorImageInfo*.....pImageInfo.........*/ nullptr,
            /*...const.VkDescriptorBufferInfo*....pBufferInfo........*/ &storageBufferDescriptorInfo,
            /*...const.VkBufferView*..............pTexelBufferView...*/ nullptr
        }
    };
    assert ((sizeof (pDescriptorUpdateWrites) / sizeof (VkWriteDescriptorSet)) == ComputeParameters::numDescriptors);

    vkUpdateDescriptorSets (/*...VkDevice....................................device.................*/ logicalDevice,
                            /*...uint32_t....................................descriptorWriteCount...*/ ComputeParameters::numDescriptors,
                            /*...const.VkWriteDescriptorSet*.................pDescriptorWrites......*/ pDescriptorUpdateWrites,
                            /*...uint32_t....................................descriptorCopyCount....*/ 0,
                            /*...const.VkCopyDescriptorSet*..................pDescriptorCopies......*/ nullptr);

    return descriptorSetHandle;
}

void RunComputeExample (const char*      pComputeShaderPath,
                        VkPhysicalDevice physicalDevice,
                        VkDevice         logicalDevice,
                        uint32_t         queueIndex,
                        VkQueue          queueHandle)
{
    uint32_t uboSize = sizeof(ComputeParameters::UboDataLayout); // fix this
    vulkanAllocatedBufferInfo uboInfo = CreateAndAllocateDeviceLocalBuffer (physicalDevice,
                                                                            logicalDevice,
                                                                            uboSize,
                                                                            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                                                            queueIndex);
    uint32_t ssboSize = 1024; // fix this
    vulkanAllocatedBufferInfo ssboInfo = CreateAndAllocateDeviceLocalBuffer (physicalDevice,
                                                                            logicalDevice,
                                                                             ssboSize,
                                                                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                                                                            queueIndex);

    vulkanAllocatedBufferInfo uboStagingBufferInfo  = CreateAndAllocaStagingBuffer (physicalDevice, logicalDevice, uboSize, queueIndex);
    vulkanAllocatedBufferInfo ssboStagingBufferInfo = CreateAndAllocaStagingBuffer (physicalDevice, logicalDevice, ssboSize, queueIndex);

    vkQueueWaitIdle (queueHandle);

    uint32_t* pUboData = static_cast<uint32_t*>(MapBufferMemory (uboStagingBufferInfo, logicalDevice));
    uint32_t numRange     = ComputeParameters::numRangeMax - ComputeParameters::numRangeMin;

    uint8_t nums[4] = { 0xAA, 0xBB, 0xCC, 0xDD };

    for (uint32_t i = 0; i < ComputeParameters::inputArraySize; i++)
    {
        uint32_t num = i;// rand () % ComputeParameters::numRangeMax;
        pUboData[i] = num;// nums[i % 4];
        printf ("inputNumArray[%u] = %x\n", i, num);
    }

    vkUnmapMemory (logicalDevice, uboStagingBufferInfo.memoryHandle);
    // Write data to buffers...

    vkQueueWaitIdle (queueHandle);

    ExecuteBuffer2BufferCopy (physicalDevice, logicalDevice, queueHandle, queueIndex, uboSize, uboStagingBufferInfo,  uboInfo);
    //ExecuteBuffer2BufferCopy (physicalDevice, logicalDevice, queueHandle, queueIndex, ssboSize, ssboStagingBufferInfo, ssboInfo);

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout      pipelineLayout      = VK_NULL_HANDLE;

    VkPipeline computePipeline = CreateComputePipeline (logicalDevice, pComputeShaderPath, &pipelineLayout, &descriptorSetLayout);

    VkCommandPool   cmdPoolHandle = VK_NULL_HANDLE;
    VkCommandBuffer cmdBuffer     = CreateAndAllocateCommandBuffer(physicalDevice, logicalDevice, queueIndex, queueHandle, &cmdPoolHandle);

    vkQueueWaitIdle (queueHandle); 

    VkDescriptorPool descriptorPool = CreateDescriptorPool (logicalDevice);
    VkDescriptorSet  descriptorset = AllocateAndWriteDescriptorSet (logicalDevice,
                                                                    descriptorPool,
                                                                    descriptorSetLayout,
                                                                    uboInfo.bufferHandle,
                                                                    ssboInfo.bufferHandle);

    RecordDispatch(physicalDevice,
                   logicalDevice,
                   cmdBuffer,
                   computePipeline,
                   pipelineLayout,
                   descriptorset,
                   1); // num workgroups to dispatch


    VkPipelineStageFlags waitStage     = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    //VkSemaphore          waitSemaphore = VkSync::SemaphoreDepot.ObtainSemaphore();

    VkSubmitInfo submitInfo =
    {
        /*...VkStructureType................sType...................*/ VK_STRUCTURE_TYPE_SUBMIT_INFO,
        /*...const.void*....................pNext...................*/ 0,
        /*...uint32_t.......................waitSemaphoreCount......*/ 0,
        /*...const.VkSemaphore*.............pWaitSemaphores.........*/nullptr, // Only execute work when this semaphore is in a signaled state
        /*...const.VkPipelineStageFlags*....pWaitDstStageMask.......*/ &waitStage,
        /*...uint32_t.......................commandBufferCount......*/ 1,
        /*...const.VkCommandBuffer*.........pCommandBuffers.........*/ &cmdBuffer,
        /*...uint32_t.......................signalSemaphoreCount....*/ 0,
        /*...const.VkSemaphore*.............pSignalSemaphores.......*/ nullptr // Semaphores that will be signaled when the command buffers for this batch have completed execution.
    };

    vkQueueSubmit (queueHandle, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle (queueHandle);

    //  copy data written to ssbo to staging buffer which we will map and read
    ExecuteBuffer2BufferCopy (physicalDevice, logicalDevice, queueHandle, queueIndex, ssboSize, ssboInfo, ssboStagingBufferInfo);

    vkQueueWaitIdle (queueHandle);

    uint32_t* pCompResults = static_cast<uint32_t*>(MapBufferMemory (ssboStagingBufferInfo, logicalDevice));

    for (uint32_t i = 0; i < ComputeParameters::inputArraySize; i++)
    {
        printf ("results[%xu] = %x\n", i, pCompResults[i]);
    }
}

#endif