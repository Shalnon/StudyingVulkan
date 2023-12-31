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

#include "StudyingVulkan.h"
#include "Vulkan_Synchronization.h"
#include <glm/gtc/matrix_transform.hpp>

extern uint32_t  numRequiredExtensions;

// struct prototypes:
struct GeometryBufferSet;
struct vulkanAllocatedBufferInfo;

struct PerSwapchainImageResources
{
    VkFence         queueSubmitFence;
    VkCommandPool   commandPool; 
    VkCommandBuffer commandBuffer;
    VkSemaphore     acquireSwapchainImageSemaphore;
    VkSemaphore     releaseSwapchainImageSemaphore;
    int32_t         queue_index;
    
    //@Note: Corresponding VkImage is provided by swapchain and not manually created
    VkImageView     presentImageViewHandle;

    VkDeviceMemory  depthImageMemoryHandle;
    VkImage         depthImageHandle;
    VkImageView     depthImageViewHandle;

    VkDeviceMemory  normalsImageMemoryHandle;
    VkImage         normalsImageHandle;
    VkImageView     normalsImageViewHandle;

    VkDeviceMemory  diffuseImageMemoryHandle;
    VkImage         diffuseImageHandle;
    VkImageView     diffuseImageViewHandle;

    VkDeviceMemory  positionImageMemoryHandle;
    VkImage         positionImageHandle;
    VkImageView     positionImageViewHandle;

    VkDeviceMemory  pResolveAttachmentsMem[SceneVulkanParameters::Subpass0::numSubpassResolveAttachments];
    VkImage         pResolveImageHandles[SceneVulkanParameters::Subpass0::numSubpassResolveAttachments];
    VkImageView     pResolveImageViews[SceneVulkanParameters::Subpass0::numSubpassResolveAttachments];


    VkFramebuffer   framebufferHandle;
    VkDescriptorSet subpass1DesciptorSetHandle;

    VkBuffer        bufferUpdatesStagingBuffer;
    VkDeviceMemory  bufferUpdatesStagingMemory;
    VkDeviceSize    updatesStagingBufferSize;
};


inline uint32_t GetPhysicalDeviceCount(VkInstance instance);
inline uint32_t GetPhysicalDeviceQueueFamilyPropertyCount(VkPhysicalDevice physicalDevice);
inline uint32_t GetPhysicalDeviceSurfaceFormatCount(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
inline uint32_t GetSwapChainImageCount(VkDevice logicalDevice, VkSwapchainKHR swapchain);


VkInstance InitializeVulkanAndCreateInstance();
VkShaderModule CreateShaderModule (VkDevice logicalDevice, const char* spvPath, bool isFrag, bool isVert);

VkSurfaceKHR CreateVkSurface(VkInstance instance, HINSTANCE  window_instance, HWND window_handle);

bool CreatePhysicalDeviceAndQueue(VkInstance                 instance,
                                 VkSurfaceKHR                surface,
                                 uint32_t                    numnRequiredExtensions,
                                 const char*                 ppRequiredDeviceExtensionNames[],
                                 VkPhysicalDeviceFeatures*   pDeviceFeatures,
                                 VkPhysicalDevice*           pPhysicalDeviceOut,
                                 VkQueue*                    pQueueOut,
                                 VkDevice*                   pLogicalDeviceOut,
                                 uint32_t*                   pQueueIndexOut);

VkSurfaceFormatKHR ChooseSwapchainFormat(VkSurfaceFormatKHR* pSupportedSwapchainSurfaceFormats,
                                         uint32_t            numSupportedFormats,
                                         const VkFormat*     pPreferredFormats,
                                         uint32_t            numPreferredFormats);

void InitializeSwapchain(VkPhysicalDevice             physicalDevice,
                         VkDevice                     logicalDevice,
                         uint32_t                     graphicsQueueIndex,
                         VkSurfaceKHR                 surface,
                         uint32_t                     numPreferredSurfaceFormats,
                         uint32_t                     numPreferredDepthFormats,
                         const VkFormat*              pPreferredSurfaceFormats,
                         const VkFormat*              pPreferredDepthFormats, 
                         VkExtent2D                   prefferredExtent,           // Dimensions of the window
                         VkSwapchainKHR               oldSwapchain,
                         VkSwapchainKHR*              pSwapchainOut,
                         VkExtent2D*                  pSwapchainExtentChosenOut,  // Swapchain format used. see related note in function body
                         VkSurfaceFormatKHR*          pSurfaceFormatOut,          // pass back the format chosen for use
                         VkFormat*                    pChosenDepthFormatOut,
                         uint32_t*                    pNumSwapchainImages,
                         PerSwapchainImageResources** ppPerSwapchainImageResourcesInOut);

VkRenderPass CreateRenderpass(VkFormat swapChainFormat, VkFormat depthFormat, VkDevice logicalDevice);

// Chooses the format that will be used for depth attachments from a list of depth formats specified by SceneVulkanParameters::preferredDeptAttachmentFormats
VkFormat ChooseDepthFormat (VkPhysicalDevice   physicalDeviceHandle,
                            uint32_t           numPrefferredDepthFormats,
                            const VkFormat*    pPreferredDepthFormats);

//Need to make one per swapchain image 
void CreateAndAllocateDepthImage (VkDevice              logicalDeviceHandle,
                                  VkPhysicalDevice      physicalDeviceHandle,
                                  uint32_t              queueFamilyIdx,
                                  VkFormat              imageFormat,
                                  VkExtent2D            imageDimensions,
                                  VkDeviceMemory*       pDepthImageMemOut,
                                  VkImage*              pImageHandleOut,
                                  VkImageView*          pDepthImageViewHandleOut,
                                  VkSampleCountFlagBits sampleCount);

//Need to make one per swapchain image 
void CreateAndAllocateColorImage (VkDevice              logicalDeviceHandle,
                                  VkPhysicalDevice      physicalDeviceHandle,
                                  uint32_t              queueFamilyIdx,
                                  VkFormat              imageFormat,
                                  VkExtent2D            imageDimensions,
                                  VkDeviceMemory*       pColorImageMemOut,
                                  VkImage*              pImageHandleOut,
                                  VkImageView*          pColorImageViewHandleOut,
                                  VkSampleCountFlagBits sampleCount,
                                  VkBool32              canBeInputAttachment); // Tells whether we want this color image to be capable of being used as an input attachment

void CreateFrameBuffers(VkDevice                    logicalDevice,
                        VkRenderPass                renderPass,
                        VkExtent2D*                 pExtent,
                        uint32_t                    numSwapchainImages,
                        PerSwapchainImageResources* pPerSwapchainImageResources);

///@Spec: Retrieve the index of the next available presentable image | https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkAcquireNextImageKHR.html
///@spec: extended info from somewhere on this page: https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#swapchain-acquire-forward-progress
///        - "If an image is acquired successfully, vkAcquireNextImageKHR must either return VK_SUCCESS or VK_SUBOPTIMAL_KHR. 
///           The implementation may return VK_SUBOPTIMAL_KHR if the swapchain no longer matches the surface properties exactly,
///           but can still be used for presentation."
///        - "When successful, vkAcquireNextImageKHR acquires a presentable image from swapchain that an application can use, 
//            and sets pImageIndex to the index of that image within the swapchain.The presentation engine
//            may not have finished reading from the image at the time it is acquired, so the application must use semaphoreand /or fence to
//            ensure that the image layout and contents are not modified until the presentation engine reads have completed."
VkResult AcuireNextSwapchainImageIdx(VkQueue                     queue,
                                     VkDevice                    logicalDevice,
                                     VkSwapchainKHR              swapchain,
                                     uint32_t*                   pSwapchainImageIdxOut,
                                     PerSwapchainImageResources* pPerFrameResources);


struct PerSubpassRenderParameters
{
    VkDescriptorSet   descriptorSet;
    VkPipeline        pipeline;
    VkPipelineLayout  pipelineLayout;
};

uint64_t ExecuteRenderLoop(VkDevice                     logicalDevice,
                           VkPhysicalDevice             physicalDevice, 
                           VkSwapchainKHR               swapchain,
                           VkQueue                      queue,
                           PerSubpassRenderParameters*  pSubpass0Parameters,
                           PerSubpassRenderParameters*  pSubpass1Parameters,
                           uint32_t                     gfxQueueIdx,
                           uint32_t                     numPreferredSwapchainFormats,
                           uint32_t                     numPreferredDepthFormats,
                           const VkFormat*              pPreferredSwapchainFormats,
                           const VkFormat*              pPreferredDepthFormats,
                           VkSurfaceKHR                 surface,
                           VkRenderPass                 renderpass,
                           PerSwapchainImageResources** ppSwapchainImageResources,
                           uint32_t*                    pNumSwapchainImages,
                           VkExtent2D*                  pExtent,
                           GeometryBufferSet*           pGeometryBufferSet,
                           UniformBufferData*           pUboData,
                           vulkanAllocatedBufferInfo*   pUniformBufferInfo);

VkSwapchainKHR ReinitializeRenderungSurface(VkDevice                     logicalDevice,
                                            VkPhysicalDevice             physicalDevice,
                                            uint32_t                     gfxQueueIndex,
                                            VkSwapchainKHR               swapchain,
                                            VkExtent2D                   swapchainExtent,
                                            uint32_t                     numPreferredDepthFormats,
                                            uint32_t                     numPreferredSwapchainFormats,
                                            const VkFormat*              pPreferredSwapchainFormats,
                                            const VkFormat*              pPreferredDepthFormats,
                                            VkSurfaceKHR                 surface,
                                            VkRenderPass                 renderpass,
                                            VkQueue                      queue,
                                            uint32_t*                    pNumSwapchainImages,
                                            PerSwapchainImageResources**  ppPerSwapchainImageResources);

VkCommandBuffer RecordRenderGeometryBufferCmds(GeometryBufferSet*          pGeometryBufferSet,
                                               PerSwapchainImageResources* pPerSwapchainImageResources,
                                               PerSubpassRenderParameters* pSubpass0Parameters,
                                               PerSubpassRenderParameters* pSubpass1Parameters,
                                               vulkanAllocatedBufferInfo*  pUniformBufferInfo,
                                               VkRenderPass                renderPass,
                                               VkExtent2D*                 pExtent,
                                               VkClearColorValue*          pColorClearValue,
                                               VkClearDepthStencilValue*   pDepthStencilClearValue);

VkResult SubmitRenderCommandBuffer (VkCommandBuffer             commandBuffer,
                                    VkQueue                     queue,
                                    PerSwapchainImageResources* pPerSwapchainImageResources);

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

            //Clear the struct
            pPerSwapchainImageResources[i]             = {};
            pPerSwapchainImageResources[i].queue_index = -1;
        }
    }
}

VkDescriptorSet AllocateAndWriteSubpass0DescriptorSet (VkDevice               logicalDevice,
                                                       VkDescriptorPool       descriptorPoolHandle,
                                                       VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                                       VkBuffer               uniformBufferHandle,
                                                       VkBuffer               storageBufferHandle);

// Allocates a descriptor set with 3 descriptors of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT and one of type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
VkDescriptorSet AllocateAndWriteSubpass1DescriptorSet (VkDevice               logicalDevice,
                                                       VkDescriptorPool       descriptorPoolHandle,
                                                       VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                                       VkImageView            diffuseColorImageViewHandle,
                                                       VkImageView            normalVectorImageViewHandle,
                                                       VkImageView            positionVectorImageViewHandle,
                                                       VkBuffer               uniformBufferHandle);


// Returns memory index of memory type that meets the requirements based on the  memRequirements and requiredPropertyFlags args
inline uint32_t ChooseMemoryTypeIdx (VkPhysicalDevice      physicalDevice,
                                     VkMemoryPropertyFlags requiredPropertyFlags, // ex: a mask of VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, etc...
                                     VkMemoryRequirements* memRequirements) //a bitmask containing one bit set for every supported memory type for the resource. Bit i is set if and only if the memory type i in the VkPhysicalDeviceMemoryProperties structure for the physical device is supported for the resource.
{
    uint32_t chosenMemTypeIdxOut = UINT32_MAX;

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
            chosenMemTypeIdxOut = memTypeIdx;
        }
    }

    return chosenMemTypeIdxOut;
}


inline glm::mat4 GetProjection (float aspectRatio,
                                float desiredFov)
{
    float zFar                  = SceneVulkanParameters::zFar;
    float zNear                 = SceneVulkanParameters::zNear;
    //float aspectRatio           = float (frameWidth) / float (frameHeight);
    float halfFov               = desiredFov / 2;
    glm::mat4 projectionFromWeb = glm::mat4 ();
    projectionFromWeb[0][0] = -aspectRatio / tanf (halfFov);
    projectionFromWeb[1][1] = -1 / tanf (halfFov); // using  -1 due to Vulkan's flipped(relative to GL) viewport
    projectionFromWeb[2][2] = zFar  / (zFar - zNear);
    projectionFromWeb[2][3] = -1.0f;
    projectionFromWeb[3][2] = ((zFar * zNear) / (zFar - zNear));

    return projectionFromWeb;
}

#endif // VULKAN_UTILS_H