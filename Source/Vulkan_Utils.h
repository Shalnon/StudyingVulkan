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

#ifndef VULKAN_UTILS_H
#define VULKAN_UTILS_H
#include "BucketSearchVisualization.h"
#include "Vulkan_Synchronization.h"


extern uint32_t  numRequiredExtensions;


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
    static const VkFormat const pPreferredSwapchainFormats[NUM_PREFERRED_SURFACE_FORMATS];
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

struct vulkanAllocatedBufferInfo
{
    VkBuffer       bufferHandle;
    VkDeviceMemory memoryHandle;
    VkDeviceSize   buffersize;
    uint32_t       offset;
};

// Returns memory index of memory type that meets the requirements based on the  memRequirements and requiredPropertyFlags args
inline uint32_t ChooseMemoryTypeIdx (VkPhysicalDevice      physicalDevice,
                                     VkMemoryPropertyFlags requiredPropertyFlags, // ex: a mask of VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, etc...
                                     VkMemoryRequirements* memRequirements); //a bitmask containing one bit set for every supported memory type for the resource. Bit i is set if and only if the memory type i in the VkPhysicalDeviceMemoryProperties structure for the physical device is supported for the resource.

vulkanAllocatedBufferInfo CreateAndAllocateDeviceLocalBuffer (VkPhysicalDevice   physicalDevice,
                                                              VkDevice           logicalDevice,
                                                              VkDeviceSize       bufferSizeInBytes,
                                                              VkBufferUsageFlags bufferUsage,
                                                              uint32_t           queueIndex);

vulkanAllocatedBufferInfo CreateAndAllocaStagingBuffer (VkPhysicalDevice physicalDevice,
                                                        VkDevice         logicalDevice,
                                                        VkDeviceSize     bufferSizeInBytes,
                                                        uint32_t         queueIndex);

void* MapBufferMemory (vulkanAllocatedBufferInfo bufferInfo,
                              VkDevice                  logicalDevice);

void ExecuteBuffer2BufferCopy (VkPhysicalDevice          physicalDevice,
                               VkDevice                  logicalDevice,
                               VkQueue                   queue,
                               uint32_t                  graphicsQueueIndex,
                               VkDeviceSize              copySize,
                               vulkanAllocatedBufferInfo srcBufferInfo,
                               vulkanAllocatedBufferInfo dstBufferInfo);

VkCommandBuffer CreateAndAllocateCommandBuffer (VkPhysicalDevice physicalDevice,
                                                VkDevice         logicalDevice,
                                                uint32_t         queueIndex,
                                                VkQueue          queueHandle,
                                                VkCommandPool*   pCommandPoolOut);

VkDescriptorSet AllocateAndWriteDescriptorSet (VkDevice               logicalDevice,
                                               VkDescriptorPool       descriptorPoolHandle,
                                               VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                               VkBuffer               uniformBufferHandle,
                                               VkBuffer               storageBufferHandle);

VkDeviceMemory AllocateVkBufferMemory (VkPhysicalDevice      physicalDevice,
                                       VkDevice              logicalDevice,
                                       VkMemoryPropertyFlags requiredPropertyFlags,
                                       VkMemoryRequirements* pBufferMemoryRequirements);


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
                           uint32_t                     frameIdx,
                           vulkanAllocatedBufferInfo    vertexBuffer,
                           uint32_t                     numTriangles);

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
                    PerSwapchainImageResources* pPerFrameArgs,
                    VkSwapchainKHR              swapchain,
                    VkQueue                     queue,
                    VkExtent2D*                 pExtent,
                    uint32_t                    frameIdx,
                    vulkanAllocatedBufferInfo   vertexBuffer,
                    uint32_t                    numTriangles);

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
            if (framebuffer != VK_NULL_HANDLE)
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
            if ((oldCommandPool   != VK_NULL_HANDLE) &&
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

#endif