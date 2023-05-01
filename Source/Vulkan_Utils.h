#pragma once
#ifndef VULKAN_UTILS_H
#define VULKAN_UTILS_H
#include "StudyingVulkan.h"
#include "Vulkan_Buffer_Tools.h"
#include "Vulkan_Synchronization.h"

// Number of vulkan extensions we need to enable/verify driver support for
extern uint32_t  numRequiredExtensions;

// VkFormats we are prepared for swpachain images to be in
static const VkFormat preferredSwapchainFormats[NUM_PREFERRED_SURFACE_FORMATS] = PREFERRED_SURRFACE_FORMATS;

// Struct for tracking info needed to create and/or modify a swapchain
struct SwapchainInfo
{
    // Swapchain handle
    VkSwapchainKHR     swapchain;

    // Dimensions of the swapchain and its surface
    VkExtent2D         chosenSwapchainExtent;

    // Surface format the swapchain surface is set up to use
    VkSurfaceFormatKHR chosenSurfaceFormat;

    // Swapchain image views
    VkImageView*        pSwapchainImageViews;
    uint32_t            numSwapchainImageViews; //TODO: double check if i need these here

    // Preferred swapchain formats that are checked against formats gpu reports as supported
    static const VkFormat pPreferredSwapchainFormats[NUM_PREFERRED_SURFACE_FORMATS];
};

struct PerSwapchainImageResources
{
    VkFence         queueSubmitFence;
    VkCommandPool   commandPool; 
    VkCommandBuffer commandBuffer;
    VkSemaphore     acquireSwapchainImageSemaphore;
    VkSemaphore     releaseSwapchainImageSemaphore;
    int32_t         queue_index;
    VkImageView     imageView;
    VkFramebuffer   framebufferHandle;
};


inline uint32_t GetPhysicalDeviceCount(VkInstance instance);
inline uint32_t GetPhysicalDeviceQueueFamilyPropertyCount(VkPhysicalDevice physicalDevice);
inline uint32_t GetPhysicalDeviceSurfaceFormatCount(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
inline uint32_t GetSwapChainImageCount(VkDevice logicalDevice, VkSwapchainKHR swapchain);


VkInstance InitializeVulkanAndCreateInstance();
VkShaderModule CreateShaderModule(VkDevice logicalDevice, const char* spvPath);

VkSurfaceKHR CreateVkSurface(VkInstance instance, HINSTANCE  window_instance, HWND window_handle);

bool CreatePhysicalDeviceAndQueue(VkInstance                 instance,
                                 VkSurfaceKHR                surface,
                                 uint32_t                    numnRequiredExtensions,
                                 char**                      ppRequiredDeviceExtensionNames,
                                 VkPhysicalDeviceFeatures*   pDeviceFeatures,
                                 VkPhysicalDevice*           pPhysicalDeviceOut,
                                 VkQueue*                    pQueueOut,
                                 VkDevice*                   pLogicalDeviceOut,
                                 uint32_t*                   pQueueIndexOut);

VkSurfaceFormatKHR ChooseSwapchainFormat(VkSurfaceFormatKHR* pSupportedSwapchainSurfaceFormats,
                                         uint32_t            numSupportedFormats,
                                         VkFormat*           pPreferredFormats,
                                         uint32_t            numPreferredFormats);

void InitializeSwapchain(VkPhysicalDevice             physicalDevice,
                         VkDevice                     logicalDevice,
                         uint32_t                     graphicsQueueIndex,
                         VkSurfaceKHR                 surface,
                         uint32_t                     numPreferredSurfaceFormats,
                         VkFormat*                    pPreferredSurfaceFormats,
                         VkExtent2D                   prefferredExtent,           // Dimensions of the window
                         VkSwapchainKHR               oldSwapchain,
                         VkSwapchainKHR*              pSwapchainOut,
                         VkExtent2D*                  pSwapchainExtentChosenOut,  // Swapchain format used. see related note in function body
                         VkSurfaceFormatKHR*          pSurfaceFormatOut,          // pass back the format used
                         uint32_t*                    pNumSwapchainImagesOut,
                         PerSwapchainImageResources** ppPerFrameResourcesOut);

VkRenderPass CreateRenderpass(VkFormat swapChainFormat, VkDevice logicalDevice);
VkPipeline   CreatePipeline(VkDevice logicalDevice, VkRenderPass renderpass, VkExtent2D* pExtent, const char* fragShaderPath, const char* vertShaderPath);

void CreateFrameBuffers(VkDevice                    logicalDevice,
                        VkRenderPass                renderPass,
                        VkExtent2D*                 pExtent,
                        uint32_t                    numSwapchainImages,
                        PerSwapchainImageResources* pPerSwapchainImageResources);

///@Spec: Retrieve the index of the next available presentable image | https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkAcquireNextImageKHR.html
/// 

VkResult AcuireNextSwapchainImageIdx(VkDevice                    logicalDevice,
                                     VkSwapchainKHR              swapchain,
                                     uint32_t*                   pSwapchainImageIdxOut,
                                     PerSwapchainImageResources* pPerFrameResources);

uint64_t ExecuteRenderLoop(VkDevice                     logicalDevice,
                           VkPhysicalDevice             physicalDevice, 
                           VkSwapchainKHR               swapchain,
                           VkQueue                      queue,
                           uint32_t                     gfxQueueIdx,
                           uint32_t                     numPreferredSwapchainFormats,
                           VkFormat*                    pPreferredSwapchainFormats,
                           VkSurfaceKHR                 surface,
                           VkRenderPass                 renderpass,
                           VkPipeline                   pipeline,
                           PerSwapchainImageResources** pPerFrameResources,
                           uint32_t*                    pNumSwapchainImages,
                           VkExtent2D*                  pExtent,
                           GeometryBufferSet*           pVertexBuffers,
                           uint32_t                     frameIdx);

VkSwapchainKHR ReinitializeRenderungSurface(VkDevice                     logicalDevice,
                                            VkPhysicalDevice             physicalDevice,
                                            uint32_t                     gfxQueueIndex,
                                            VkSwapchainKHR               swapchain,
                                            uint32_t                     swapchainWidth,
                                            uint32_t                     swapchainHeight,
                                            uint32_t                     numPreferredSwapchainFormats,
                                            VkFormat*                    pPreferredSwapchainFormats,
                                            VkSurfaceKHR                 surface,
                                            VkRenderPass                 renderpass,
                                            VkQueue                      queue,
                                            uint32_t*                    numSwapchainImages,
                                            PerSwapchainImageResources** ppPerSwapchainImageResources);

void RenderTriangle(uint32_t                    swapChainImageIdx,
                    VkRenderPass                renderPass,
                    VkPipeline                  pipeline,
                    PerSwapchainImageResources* pPerSwapchainImageResources,
                    VkSwapchainKHR              swapchain,
                    VkQueue                     queue,
                    VkExtent2D*                 pExtent,
                    const VkBuffer              vertexBufferHandle,
                    uint32_t                    frameIdx);

VkResult PresentImage(VkSwapchainKHR swapchain,
                      uint32_t       swapchainImageIndex,
                      VkSemaphore    swapchainImageReleaseSemaphore,
                      VkQueue        queue);

inline void ResestPerSwapchainImageResources (VkDevice                      logicalDevice,
                                              PerSwapchainImageResources*   pPerSwapchainImageResources,
                                              uint32_t                      numSwapchainImages)
{
    if (pPerSwapchainImageResources != 0)
    {
        for (uint32_t i = 0; i < numSwapchainImages; i++)
        {
            VkFramebuffer framebuffer = pPerSwapchainImageResources[i].framebufferHandle;
            if (framebuffer == VK_NULL_HANDLE)
            {
                vkDestroyFramebuffer (logicalDevice, framebuffer, nullptr);
            }

            VkFence oldFence = pPerSwapchainImageResources[i].queueSubmitFence;
            if (oldFence != VK_NULL_HANDLE)
            {
                vkDestroyFence (logicalDevice, oldFence, nullptr);
            }

            VkCommandPool   oldCommandPool   = pPerSwapchainImageResources[i].commandPool;
            VkCommandBuffer oldCommandBuffer = pPerSwapchainImageResources[i].commandBuffer;
            if ((oldCommandPool != VK_NULL_HANDLE) &&
                (oldCommandBuffer != VK_NULL_HANDLE))
            {
                vkFreeCommandBuffers (logicalDevice, oldCommandPool, 1, &oldCommandBuffer);
                vkDestroyCommandPool (logicalDevice, oldCommandPool, nullptr);
            }

            VkSemaphore oldAcquireSeamphore = pPerSwapchainImageResources[i].acquireSwapchainImageSemaphore;
            if (oldAcquireSeamphore != VK_NULL_HANDLE)
            {
                VkSync::SemaphoreDepot.DepositUnusedSemaphore (oldAcquireSeamphore);
            }

            VkSemaphore oldReleaseSemaphore = pPerSwapchainImageResources[i].releaseSwapchainImageSemaphore;
            if (oldReleaseSemaphore != VK_NULL_HANDLE)
            {
                VkSync::SemaphoreDepot.DepositUnusedSemaphore (oldReleaseSemaphore);
            }

            //Clear the struct
            pPerSwapchainImageResources[i]             = {};
            pPerSwapchainImageResources[i].queue_index = -1;
        }
    }
}

inline void DrawGeometry (uint32_t                    swapChainImageIdx,
                          VkRenderPass                renderPass,
                          VkPipeline                  pipeline,
                          PerSwapchainImageResources* pPerSwapchainImageResources,
                          VkSwapchainKHR              swapchain,
                          VkQueue                     queue,
                          VkExtent2D*                 pExtent,
                          GeometryBufferSet*          pGeometryBuffers, // contains vertex and index buffer, their memory, and vert/prim counts;
                          uint32_t                    frameIdx)
{
    VkClearValue clearValArray[] =
    {
        {0.01f, 0.01f, 0.033f, 1.0f},
        {0.01f, 0.033f, 0.01f, 1.0f},
        {0.033f, 0.01f, 0.01f, 1.0f}
    };

    VkFramebuffer   framebuffer   = pPerSwapchainImageResources[swapChainImageIdx].framebufferHandle;
    VkCommandBuffer commandBuffer = pPerSwapchainImageResources[swapChainImageIdx].commandBuffer;

    VkCommandBufferBeginInfo cmdBufferBeginInfo =
    {
        /*...VkStructureType..........................sType................*/ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        /*...const.void*..............................pNext................*/ 0,
        /*...VkCommandBufferUsageFlags................flags................*/ VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        /*...const.VkCommandBufferInheritanceInfo*....pInheritanceInfo.....*/ 0
    };

    // Begin command buffer
    vkBeginCommandBuffer (commandBuffer, &cmdBufferBeginInfo);

    VkRect2D renderArea =
    {
        /*...VkOffset2D....offset....*/ {0, 0},
        /*...VkExtent2D....extent....*/ *pExtent
    };

    VkRenderPassBeginInfo renderPassBeginInfo =
    {
        /*...VkStructureType........sType..............*/ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        /*...const.void*............pNext..............*/ 0,
        /*...VkRenderPass...........renderPass.........*/ renderPass,
        /*...VkFramebuffer..........framebuffer........*/ framebuffer,
        /*...VkRect2D...............renderArea.........*/ renderArea,
        /*...uint32_t...............clearValueCount....*/ 1,
        /*...const.VkClearValue*....pClearValues.......*/ &clearValArray[frameIdx % 3]
    };

    vkCmdBeginRenderPass (commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline (commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkViewport viewport =
    {
        /*...float....x..........*/ 0,
        /*...float....y..........*/ 0,
        /*...float....width......*/ static_cast<float>(pExtent->width),
        /*...float....height.....*/ static_cast<float>(pExtent->height),
        /*...float....minDepth...*/ 0.0,
        /*...float....maxDepth...*/ 1.0
    };

    vkCmdSetViewport (commandBuffer, 0, 1, &viewport);

    VkRect2D scissor =
    {
        /*...VkOffset2D....offset...*/ {0, 0},
        /*...VkExtent2D....extent...*/ *pExtent
    };

    vkCmdSetScissor (commandBuffer, 0, 1, &scissor);

    VkBuffer     vertexBuffer  = pGeometryBuffers->vertexBufferInfo.bufferHandle;
    VkBuffer     indexBuffer   = pGeometryBuffers->indexBufferInfo.bufferHandle;
    VkDeviceSize offsets[]     = { 0 };

    vkCmdBindVertexBuffers ( /*..VkCommandBuffer.......commandBuffer....*/ commandBuffer,
                            /*..uint32_t...............firstBinding.....*/ 0,
                            /*..uint32_t...............bindingCount.....*/ 1,
                            /*..const.VkBuffer*........pBuffers.........*/ &vertexBuffer,
                            /*..const.VkDeviceSize*....pOffsets.........*/ offsets);

    vkCmdDraw (commandBuffer, 3, 1, 0, 0);


   // vkCmdBindIndexBuffer (/*...VkCommandBuffer..........commandBuffer....*/ commandBuffer,
   //                       /*...VkBuffer.................buffer...........*/ indexBuffer,
   //                       /*...VkDeviceSize.............offset...........*/ 0,
   //                       /*...VkIndexType..............indexType........*/ VK_INDEX_TYPE_UINT32);
   //
   // vkCmdDrawIndexed (/*...VkCommandBuffer.......commandBuffer......*/ commandBuffer,
   //                   /*...uint32_t..............indexCount.........*/ pGeometryBuffers->numTriangles * NUM_VERTICES_PER_TRIANGLE,
   //                   /*...uint32_t..............instanceCount......*/ 1,
   //                   /*...uint32_t..............firstIndex.........*/ 0,
   //                   /*...int32_t...............vertexOffset.......*/ 0,
   //                   /*...uint32_t..............firstInstance......*/ 0);

    vkCmdEndRenderPass (commandBuffer);

    VkResult result = vkEndCommandBuffer (commandBuffer);
    assert (result == VK_SUCCESS);

    if (pPerSwapchainImageResources[swapChainImageIdx].releaseSwapchainImageSemaphore == VK_NULL_HANDLE)
    {
        pPerSwapchainImageResources[swapChainImageIdx].releaseSwapchainImageSemaphore = VkSync::SemaphoreDepot.ObtainSemaphore ();
    }

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo =
    {
        /*...VkStructureType................sType...................*/ VK_STRUCTURE_TYPE_SUBMIT_INFO,
        /*...const.void*....................pNext...................*/ 0,
        /*...uint32_t.......................waitSemaphoreCount......*/ 1,
        /*...const.VkSemaphore*.............pWaitSemaphores.........*/ &(pPerSwapchainImageResources[swapChainImageIdx].acquireSwapchainImageSemaphore), // Only execute work when this semaphore is in a signaled state
        /*...const.VkPipelineStageFlags*....pWaitDstStageMask.......*/ &waitStage,
        /*...uint32_t.......................commandBufferCount......*/ 1,
        /*...const.VkCommandBuffer*.........pCommandBuffers.........*/ &commandBuffer,
        /*...uint32_t.......................signalSemaphoreCount....*/ 1,
        /*...const.VkSemaphore*.............pSignalSemaphores.......*/ &(pPerSwapchainImageResources[swapChainImageIdx].releaseSwapchainImageSemaphore) // Semaphores that will be signaled when the command buffers for this batch have completed execution.
    };

    result = vkQueueSubmit (queue, 1, &submitInfo, pPerSwapchainImageResources[swapChainImageIdx].queueSubmitFence);
    assert (result == VK_SUCCESS);
}



#endif