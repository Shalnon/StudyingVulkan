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
#ifndef VULKAN_COMPUTE_CPP
#define VULKAN_COMPUTE_CPP

#include "Vulkan_compute.h"
#include <time.h>

size_t ComputeParameters::binSize = ComputeParameters::defaultBinSize; // |EXTERN| bin size initialization. default value may be overwritten in main if a
                                                                       //          different bin size is passed to the application via a command line arg

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

inline void RecordDispatch (VkPhysicalDevice physicalDevice,
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

std::vector<Bar> CreateBarGraphMesh (uint32_t* pBarHeights, uint32_t numBars)
{
    static const float s_screenWidth = 2;
    static const float s_floorHeight = 0.9;

    std::vector<Bar> bars = {};

    uint32_t tallestBar  = 0;
    uint32_t smallestBar = 255;
    for (uint32_t i = 0; i < numBars; i++)
    {
        if (pBarHeights[i] > tallestBar)  { tallestBar = pBarHeights[i]; }
        if (pBarHeights[i] < smallestBar) { smallestBar = pBarHeights[i]; }
    }


    float heightUnit        = 1.8 / static_cast<float>(tallestBar);
    float marginBetweenBars = 0.003;
    float barWidth          = (s_screenWidth / numBars);
    float strideBetweenBars = barWidth - marginBetweenBars;

    glm::vec3 currentBarPosition = glm::vec3 (-0.9f, -1.9f, 0.0f);

    for (uint32_t i = 0; i < numBars; i++)
    {
        float currentBarsHeight = pBarHeights[i] * heightUnit;

        bars.push_back (Bar (currentBarsHeight, barWidth, currentBarPosition, s_floorHeight));

        currentBarPosition.x += strideBetweenBars;
    }


    return bars;
}


void RunComputeExample (const char*            pComputeShaderPath,
                        VkPhysicalDevice       physicalDevice,
                        VkDevice               logicalDevice,
                        uint32_t               queueIndex,
                        VkQueue                queueHandle,
                        std::vector<uint32_t>& histogram_out)
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

    srand (time (nullptr));

    vulkanAllocatedBufferInfo uboStagingBufferInfo  = CreateAndAllocaStagingBuffer (physicalDevice, logicalDevice, uboSize, queueIndex);
    vulkanAllocatedBufferInfo ssboStagingBufferInfo = CreateAndAllocaStagingBuffer (physicalDevice, logicalDevice, ssboSize, queueIndex);
    uint32_t numPackedChars = ComputeParameters::minInputArraySize +
                              ( rand() % (ComputeParameters::maxInputArraySize - ComputeParameters::minInputArraySize));

    vkQueueWaitIdle (queueHandle);

    ComputeParameters::UboDataLayout* pUboData = static_cast<ComputeParameters::UboDataLayout*>(MapBufferMemory (uboStagingBufferInfo, logicalDevice));

    pUboData->numPackedChars = numPackedChars; //ComputeParameters::inputArraySize;
    pUboData->binSize        = ComputeParameters::binSize;
    printf ("Input array will contain %u packed uint8 numbers\n", pUboData->numPackedChars);

    for (uint8_t i = 0; i < numPackedChars; i++)
    {
        uint8_t num = rand() % ComputeParameters::numRangeMax;
        pUboData->numArray[i] = num;// nums[i % 4];
        printf ("inputNumArray[%u] = %x\n", i, num);
    }

    vkUnmapMemory (logicalDevice, uboStagingBufferInfo.memoryHandle);

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
        /*...const.VkSemaphore*.............pWaitSemaphores.........*/ nullptr, // Only execute work when this semaphore is in a signaled state
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

    uint32_t numBins = numPackedChars / ComputeParameters::binSize;
    for (uint32_t i = 0; i < numBins; i++)
    {
        uint32_t binRangeMin = ComputeParameters::binSize * i;

        printf ("bin %u (%u - %u): %u\n", i, binRangeMin, binRangeMin + ComputeParameters::binSize,pCompResults[i]);
        histogram_out.push_back (pCompResults[i]);
    }

    vkUnmapMemory (logicalDevice, ssboStagingBufferInfo.memoryHandle);
}
#endif