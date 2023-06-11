#ifndef VULKAN_UTILS_H
#define VULKAN_UTILS_H
#include "StudyingVulkan.h"
#include "Vulkan_Synchronization.h"


extern uint32_t  numRequiredExtensions;

// struct prototypes:
struct GeometryBufferSet;

struct PerSwapchainImageResources
{
    VkFence         queueSubmitFence;
    VkCommandPool   commandPool; 
    VkCommandBuffer commandBuffer;
    VkSemaphore     acquireSwapchainImageSemaphore;
    VkSemaphore     releaseSwapchainImageSemaphore;
    int32_t         queue_index;
    
    // Corresponding VkImage is provided by swapchain and not manually created
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

    VkFramebuffer   framebufferHandle;
    VkDescriptorSet subpass1DesciptorSetHandle;
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
                                 char**                      ppRequiredDeviceExtensionNames,
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
                         VkSurfaceFormatKHR*          pSurfaceFormatOut,          // pass back the format used
                         VkFormat*                    pChosenDepthFormatOut,
                         uint32_t*                    pNumSwapchainImages,
                         PerSwapchainImageResources** ppPerSwapchainImageResources);

VkRenderPass CreateRenderpass(VkFormat swapChainFormat, VkFormat depthFormat, VkDevice logicalDevice);

VkFormat ChooseDepthFormat (VkPhysicalDevice   physicalDeviceHandle,
                            uint32_t           numPrefferredDepthFormats,
                            const VkFormat*    pPreferredDepthFormats);

//Need to make one per swapchain image 
void CreateAndAllocateDepthImage (VkDevice            logicalDeviceHandle,
                                  VkPhysicalDevice    physicalDeviceHandle,
                                  uint32_t            queueFamilyIdx,
                                  VkFormat            imageFormat,
                                  VkExtent2D          imageDimensions,
                                  VkDeviceMemory*     pDepthImageMemOut,
                                  VkImage*            pImageHandleOut,
                                  VkImageView*        pDepthImageViewHandleOut);

//Need to make one per swapchain image 
void CreateAndAllocateColorImage (VkDevice           logicalDeviceHandle,
                                  VkPhysicalDevice   physicalDeviceHandle,
                                  uint32_t           queueFamilyIdx,
                                  VkFormat           imageFormat,
                                  VkExtent2D         imageDimensions,
                                  VkDeviceMemory*    pColorImageMemOut,
                                  VkImage*           pImageHandleOut,
                                  VkImageView*       pColorImageViewHandleOut);

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
                           uint32_t                     frameIdx);

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
                                            PerSwapchainImageResources** ppPerSwapchainImageResources);

VkCommandBuffer RecordRenderGeometryBufferCmds(GeometryBufferSet*          pGeometryBufferSet,
                                               PerSwapchainImageResources* pPerSwapchainImageResources,
                                               PerSubpassRenderParameters* pSubpass0Parameters,
                                               PerSubpassRenderParameters* pSubpass1Parameters,
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

// Allocates a descriptor set with 3 descriptors of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
VkDescriptorSet AllocateAndWriteSubpass1DescriptorSet (VkDevice               logicalDevice,
                                                       VkDescriptorPool       descriptorPoolHandle,
                                                       VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                                       VkImageView            diffuseColorImageViewHandle,
                                                       VkImageView            normalVectorImageViewHandle,
                                                       VkImageView            depthStencilImageViewHandle,
                                                       VkBuffer               uniformBufferHandle);


// Returns memory index of memory type that meets the requirements based on the  memRequirements and requiredPropertyFlags args
inline uint32_t ChooseMemoryTypeIdx (VkPhysicalDevice      physicalDevice,
                                     VkMemoryPropertyFlags requiredPropertyFlags, // ex: a mask of VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, etc...
                                     VkMemoryRequirements* memRequirements) //a bitmask and contains one bit set for every supported memory type for the resource. Bit i is set if and only if the memory type i in the VkPhysicalDeviceMemoryProperties structure for the physical device is supported for the resource.
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
            //  *pChosenHeapIndexOut = physicalDeviceMemoryProperties.memoryTypes[memTypeIdx].heapIndex;
        }
    }

    return chosenMemTypeIdxOut;
}

#endif // VULKAN_UTILS_H