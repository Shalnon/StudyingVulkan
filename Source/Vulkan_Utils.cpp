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
#ifndef VULKAN_UTILS_CPP
#define VULKAN_UTILS_CPP

// This needs to be defined for volk to work
#define VOLK_IMPLEMENTATION

#include "Vulkan_Utils.h"
#include "Vulkan_enum_strings.h"
#include "Vulkan_Buffer_Tools.h"
#include "Vulkan_Descriptor_Tools.h"
#include <glm/glm.hpp>

inline uint32_t GetPhysicalDeviceCount(VkInstance instance)
{
    VkResult         result;
    VkPhysicalDevice physicalDevice = 0;

    uint32_t deviceCount = 0;
    result = vkEnumeratePhysicalDevices(instance, &deviceCount, 0);

    assert(result == VK_SUCCESS);

    return deviceCount;
}

inline uint32_t GetPhysicalDeviceQueueFamilyPropertyCount(VkPhysicalDevice physicalDevice)
{
    uint32_t                queueFamilyPropertyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, 0);

    return queueFamilyPropertyCount;
}

inline uint32_t GetPhysicalDeviceSurfaceFormatCount(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, 0);

    return formatCount;
}

inline uint32_t GetSwapChainImageCount(VkDevice logicalDevice, VkSwapchainKHR swapchain)

{
    uint32_t imageCount = 0;
    VkResult  result    = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, 0);

    assert(result == VK_SUCCESS);
    assert(imageCount > 0);

    return imageCount;
}

VkInstance InitializeVulkanAndCreateInstance()
{
    VkResult   result = VK_INCOMPLETE;
    VkInstance instance = VK_NULL_HANDLE;

#if defined(USE_VOLK)
    result = volkInitialize();
#else
    HMODULE vkModule = LoadLibrary(L"vulkan-1.dll");
    
    assert(vkModule);

#define VK_EXPORTED_FUNCTION( fun )                                                   \
    if( !(fun = (PFN_##fun)GetProcAddress( VulkanLibrary, #fun )) ) {                \
      std::cout << "Could not load exported function: " << #fun << "!" << std::endl;  \

    g_vkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(GetProcAddress(nullptr, "vkCreateInstance"));
#endif


    if (result == VK_SUCCESS)
    {
        const char* win32SurfaceExtensionName   = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
        const char* vkKhrSurfaceExtensionName   = VK_KHR_SURFACE_EXTENSION_NAME;
        const char* const* ppEnabledLayerNames = 0;
        const char*        ppExtensionNames[]  = { vkKhrSurfaceExtensionName , win32SurfaceExtensionName };

        VkApplicationInfo  applicationInfo     =
        {
            /*VkStructureType....sType......................*/ VK_STRUCTURE_TYPE_APPLICATION_INFO,
            /*const.void*........pNext......................*/ 0,
            /*const.char*........pApplicationName...........*/ "StudyingVulkan",
            /*uint32_t...........applicationVersion.........*/ 0,
            /*const.char*........pEngineName................*/ "TheFucQEngine",
            /*uint32_t...........engineVersion..............*/ 0,
            /*uint32_t...........apiVersion.................*/ VK_API_VERSION_1_3
        };

        uint32_t             numLayers          = 0;
        VkInstanceCreateInfo instanceCreateInfo =
        {
            /*VkStructureType.............sType..........................*/ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            /*const.void*.................pNext..........................*/ 0,
            /*VkInstanceCreateFlags.......flags..........................*/ 0,
            /*const.VkApplicationInfo*....pApplicationInfo...............*/ &applicationInfo,
            /*uint32_t....................enabledLayerCount..............*/ numLayers,
            /*const.char*.const*..........ppEnabledLayerNames............*/ ppEnabledLayerNames,
            /*uint32_t....................enabledExtensionCount..........*/ sizeof(ppExtensionNames) / sizeof(const char*),
            /*const.char*.const*..........ppEnabledExtensionNames........*/ ppExtensionNames
        };

        VkResult result = vkCreateInstance(/*const.VkInstanceCreateInfo*............pCreateInfo.....*/ &instanceCreateInfo,
                                           /*const.VkAllocationCallbacks*...........pAllocator......*/ 0,
                                           /*VkInstance*............................pInstance.......*/ &instance);
#if defined(USE_VOLK)
        if (result == VK_SUCCESS)
        {
            volkLoadInstance(instance);
        }
        else
        {
            printf("Vk instance creation unsuccessfull\n");
        }
#else
    //@TODO: Add more API functions
    g_vkEnumeratePhysicalDevices                   =    reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
    g_vkGetPhysicalDeviceQueueFamilyProperties     =    reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties"));
    g_vkGetPhysicalDeviceSurfaceFormatsKHR         =    reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
    g_vkGetSwapchainImagesKHR                      =    reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetInstanceProcAddr(instance, "vkGetSwapchainImagesKHR"));
    g_vkCreateWin32SurfaceKHR                      =    reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR"));
    g_vkEnumeratePhysicalDevices                   =    reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
    g_vkGetPhysicalDeviceQueueFamilyProperties     =    reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties"));
    g_vkGetPhysicalDeviceSurfaceSupportKHR         =    reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
    g_vkCreateDevice                               =    reinterpret_cast<PFN_vkCreateDevice>(vkGetInstanceProcAddr(instance, "vkCreateDevice"));
    g_vkGetDeviceQueue                             =    reinterpret_cast<PFN_vkGetDeviceQueue>(vkGetInstanceProcAddr(instance, "vkGetDeviceQueue"));
    g_vkGetPhysicalDeviceSurfaceCapabilitiesKHR    =    reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
    g_vkGetPhysicalDeviceSurfaceFormatsKHR         =    reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
    g_vkCreateSwapchainKHR                         =    reinterpret_cast<PFN_vkCreateSwapchainKHR>(vkGetInstanceProcAddr(instance, "vkCreateSwapchainKHR"));
    g_vkGetSwapchainImagesKHR                      =    reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetInstanceProcAddr(instance, "vkGetSwapchainImagesKHR"));
    g_vkCreateFence                                =    reinterpret_cast<PFN_vkCreateFence>(vkGetInstanceProcAddr(instance, "vkCreateFence"));
    g_vkCreateCommandPool                          =    reinterpret_cast<PFN_vkCreateCommandPool>(vkGetInstanceProcAddr(instance, "vkCreateCommandPool"));
    g_vkAllocateCommandBuffers                     =    reinterpret_cast<PFN_vkAllocateCommandBuffers>(vkGetInstanceProcAddr(instance, "vkAllocateCommandBuffers"));
    g_vkCreateImageView                            =    reinterpret_cast<PFN_vkCreateImageView>(vkGetInstanceProcAddr(instance, "vkCreateImageView"));
    g_vkCreateRenderPass                           =    reinterpret_cast<PFN_vkCreateRenderPass>(vkGetInstanceProcAddr(instance, "vkCreateRenderPass"));
    g_vkCreateShaderModule                         =    reinterpret_cast<PFN_vkCreateShaderModule>(vkGetInstanceProcAddr(instance, "vkCreateShaderModule"));
    g_vkCreatePipelineLayout                       =    reinterpret_cast<PFN_vkCreatePipelineLayout>(vkGetInstanceProcAddr(instance, "vkCreatePipelineLayout"));
    g_vkCreateGraphicsPipelines                    =    reinterpret_cast<PFN_vkCreateGraphicsPipelines>(vkGetInstanceProcAddr(instance, "vkCreateGraphicsPipelines"));
#endif


    }

    return instance;
}

VkSurfaceKHR CreateVkSurface(VkInstance instance, HINSTANCE window_instance, HWND window_handle)
{
    assert(window_handle != 0);
    assert(window_handle != 0);

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo =
    {
        /*..VkStructureType.................sType.......*/ VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        /*..const.void*.....................pNext.......*/ 0,
        /*..VkWin32SurfaceCreateFlagsKHR....flags.......*/ 0,
        /*..HINSTANCE.......................hinstance...*/ window_instance,
        /*..HWND............................hwnd........*/ window_handle
    };

    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkResult     result  = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, 0, &surface);

    assert((result == VK_SUCCESS) && (surface != VK_NULL_HANDLE));

    return surface;
}

bool CreatePhysicalDeviceAndQueue(VkInstance                instance,
                                  VkSurfaceKHR              surface,
                                  uint32_t                  numnRequiredExtensions,
                                  const char*               ppRequiredDeviceExtensionNames[],
                                  VkPhysicalDeviceFeatures* pDeviceFeatures,  //get rid of this arg
                                  VkPhysicalDevice*         pPhysicalDeviceOut,
                                  VkQueue*                  pQueueOut,
                                  VkDevice*                 pLogicalDeviceOut,
                                  uint32_t*                 pQueueIndexOut)
{
    VkResult          result           = VK_INCOMPLETE;
    uint32_t          deviceCount      = GetPhysicalDeviceCount(instance);
    VkPhysicalDevice* pPhysicalDevices = static_cast<VkPhysicalDevice*>(calloc(sizeof(VkPhysicalDevice), deviceCount));
    assert(pPhysicalDevices);

    //Fill in pPhysicalDevices array
    result = vkEnumeratePhysicalDevices(instance, &deviceCount, pPhysicalDevices);

    assert(result == VK_SUCCESS);

    uint32_t chosenDeviceIndex;
    uint32_t chosenQueueFamilyIndex;
    bool     FoundSuitableQueueDeviceCombo = false;
    uint32_t deviceIdx                     = 0;

    VkPhysicalDeviceProperties       physicalDeviceProperties       = {};
    VkPhysicalDeviceFeatures2        physicalDeviceFeatures2        = {};
    VkPhysicalDeviceVulkan13Features physicalDeviceVulkan13Features = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES, };

    while ((FoundSuitableQueueDeviceCombo == false) && (deviceIdx < deviceCount))
    {
        physicalDeviceProperties = {};
        physicalDeviceFeatures2  = {};

        if (SceneVulkanParameters::enableSynchronization2Feature == VK_TRUE)
        {
            physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
            physicalDeviceFeatures2.pNext = &physicalDeviceVulkan13Features;
        }

        VkPhysicalDevice physicalDevice = pPhysicalDevices[deviceIdx];

        vkGetPhysicalDeviceFeatures (physicalDevice, &(physicalDeviceFeatures2.features)); // According to the spec, the call to vkGetPhysicalDeviceFeatures2 should fill in physicalDeviceFeatures2.features, however on my current driver leaves it empty(rtx3080). So this explicit call is needed
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        if (SceneVulkanParameters::enableSynchronization2Feature == VK_TRUE)
        {
            vkGetPhysicalDeviceFeatures2 (physicalDevice, &physicalDeviceFeatures2);  // This is supposed to fill in physicalDeviceFeatures2, and  physicalDeviceVulkan13Features which is in the pNext chain of physicalDeviceFeatures2.
        }

        printf ("Device api version: %u.%u.%u\n", VK_VERSION_MAJOR(physicalDeviceProperties.apiVersion),
                                                  VK_API_VERSION_MINOR(physicalDeviceProperties.apiVersion),
                                                  VK_API_VERSION_PATCH(physicalDeviceProperties.apiVersion));


        uint32_t                 queueFamilyPropertyCount = GetPhysicalDeviceQueueFamilyPropertyCount(physicalDevice);//queue family property count is the count of queue families i think
        VkQueueFamilyProperties* pQueueFamilyProperties   = static_cast<VkQueueFamilyProperties*>(calloc(queueFamilyPropertyCount, sizeof(VkQueueFamilyProperties)));

        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, pQueueFamilyProperties);

        // Loop over queue families supported by the device
        for (uint32_t queueIndex = 0; queueIndex < queueFamilyPropertyCount; queueIndex++)
        {
            VkBool32 deviceSupportsSurface = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueIndex, surface, &deviceSupportsSurface);

            assert(pQueueFamilyProperties != 0);


            if ((deviceSupportsSurface == TRUE) &&
                (pQueueFamilyProperties[queueIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT))
            {
                chosenQueueFamilyIndex        = queueIndex;
                chosenDeviceIndex             = deviceIdx;
                FoundSuitableQueueDeviceCombo = true;
                break;
            }
        }

        deviceIdx++;
    }

    assert(FoundSuitableQueueDeviceCombo == true);

    *pPhysicalDeviceOut = pPhysicalDevices[chosenDeviceIndex];

    float queuePriority = 1.0;
    VkDeviceQueueCreateInfo queueInfo
    {
        /*..VkStructureType.............sType................*/ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        /*..const.void*.................pNext................*/ 0,
        /*..VkDeviceQueueCreateFlags....flags................*/ 0,
        /*..uint32_t....................queueFamilyIndex.....*/ chosenQueueFamilyIndex,
        /*..uint32_t....................queueCount...........*/ 1,
        /*..const.float*................pQueuePriorities.....*/ &queuePriority
    };

    if (physicalDeviceVulkan13Features.synchronization2 == VK_TRUE) { printf ("Vulkan13Features.synchronization2 is supported!\n"); }

    // Assert to make sure that synchronization 2 is supported if were planning on using it.
    assert ((physicalDeviceVulkan13Features.synchronization2 == VK_TRUE) || (SceneVulkanParameters::enableSynchronization2Feature == VK_FALSE));

    physicalDeviceVulkan13Features =
    {
        /*..VkStructureType..sType................................................*/ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        /*..void*............pNext................................................*/ VK_FALSE,
        /*..VkBool32.........robustImageAccess....................................*/ VK_FALSE,
        /*..VkBool32.........inlineUniformBlock...................................*/ VK_FALSE,
        /*..VkBool32.........descriptorBindingInlineUniformBlockUpdateAfterBind...*/ VK_FALSE,
        /*..VkBool32.........pipelineCreationCacheControl.........................*/ VK_FALSE,
        /*..VkBool32.........privateData..........................................*/ VK_FALSE,
        /*..VkBool32.........shaderDemoteToHelperInvocation.......................*/ VK_FALSE,
        /*..VkBool32.........shaderTerminateInvocation............................*/ VK_FALSE,
        /*..VkBool32.........subgroupSizeControl..................................*/ VK_FALSE,
        /*..VkBool32.........computeFullSubgroups.................................*/ VK_FALSE,
        /*..VkBool32.........synchronization2.....................................*/ SceneVulkanParameters::enableSynchronization2Feature,
        /*..VkBool32.........textureCompressionASTC_HDR...........................*/ VK_FALSE,
        /*..VkBool32.........shaderZeroInitializeWorkgroupMemory..................*/ VK_FALSE,
        /*..VkBool32.........dynamicRendering.....................................*/ VK_FALSE,
        /*..VkBool32.........shaderIntegerDotProduct..............................*/ VK_FALSE,
        /*..VkBool32.........maintenance4.........................................*/ VK_FALSE,
    };

    VkPhysicalDeviceFeatures featuresToEnable             = {};
                             featuresToEnable.depthBounds = true;

    const void*        pNextChain       = SceneVulkanParameters::enableSynchronization2Feature ? &physicalDeviceVulkan13Features : nullptr;
    VkDeviceCreateInfo deviceCreateInfo =
    {
        /*....VkStructureType....................sType.......................*/ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        /*....const.void*........................pNext.......................*/ pNextChain,
        /*....VkDeviceCreateFlags................flags.......................*/ 0,
        /*....uint32_t...........................queueCreateInfoCount........*/ 1,
        /*....const.VkDeviceQueueCreateInfo*.....pQueueCreateInfos...........*/ &queueInfo,
        /*....uint32_t...........................enabledLayerCount...........*/ 0,
        /*....const.char*.const*.................ppEnabledLayerNames.........*/ 0,
        /*....uint32_t...........................enabledExtensionCount.......*/ numnRequiredExtensions,
        /*....const.char*.const*.................ppEnabledExtensionNames.....*/ ppRequiredDeviceExtensionNames,
        /*....const.VkPhysicalDeviceFeatures*....pEnabledFeatures............*/ &featuresToEnable
    };

    *pQueueOut         = VK_NULL_HANDLE;
    *pLogicalDeviceOut = VK_NULL_HANDLE;

    result = vkCreateDevice(*pPhysicalDeviceOut, &deviceCreateInfo, 0, pLogicalDeviceOut);
    volkLoadDevice(*pLogicalDeviceOut);

    assert(result == VK_SUCCESS);

    vkGetDeviceQueue(/*...VkDevice......device.............*/ *pLogicalDeviceOut,
                     /*...uint32_t......queueFamilyIndex...*/ chosenQueueFamilyIndex,
                     /*...uint32_t......queueIndex.........*/ 0,
                     /*...VkQueue*......pQueue)............*/ pQueueOut);
         
    *pQueueIndexOut = chosenQueueFamilyIndex;

    return (*pQueueOut != VK_NULL_HANDLE) && (pLogicalDeviceOut != VK_NULL_HANDLE);
}

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
                         VkSurfaceFormatKHR*          pSurfaceFormatOut,          // pass back the color format and colorspace used
                         VkFormat*                    pChosenDepthFormatOut,      // pass back the depth format that the depth images will be using
                         uint32_t*                    pNumSwapchainImages,
                         PerSwapchainImageResources**  ppPerSwapchainImageResourcesInOut)
{
    // Get Surface capabilities
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    VkResult                 result              = VK_SUCCESS;

    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
    assert(result == VK_SUCCESS);
    assert(surfaceCapabilities.maxImageCount > 0);

    //Choose a surface format to use with swapchain
    uint32_t                 supportedFormatCount              = GetPhysicalDeviceSurfaceFormatCount(physicalDevice, surface);
    VkSurfaceFormatKHR*      pSupportedSwapchainSurfaceFormats = reinterpret_cast<VkSurfaceFormatKHR*>(calloc(sizeof(VkSurfaceFormatKHR), supportedFormatCount));

    //Get supported swapchain formats
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice,
                                                  surface,
                                                  &supportedFormatCount,
                                                  pSupportedSwapchainSurfaceFormats);

    //Choose a swapchain format
    const VkSurfaceFormatKHR chosenSwapchainFormat = ChooseSwapchainFormat(pSupportedSwapchainSurfaceFormats, 
                                                                           supportedFormatCount,
                                                                           pPreferredSurfaceFormats,
                                                                           numPreferredSurfaceFormats);

    ///@Note: The spec says: "currentExtent is the current width and height of the surface, or the special value (0xFFFFFFFF, 0xFFFFFFFF) indicating that the surface size will be determined by the extent of a swapchain targeting the surface."
    const VkExtent2D swapchainExtent = (surfaceCapabilities.currentExtent.height == 0xFFFFFFFF) ? prefferredExtent : surfaceCapabilities.currentExtent;

    // Saving this for later
    *pSwapchainExtentChosenOut = swapchainExtent;
    *pSurfaceFormatOut         = chosenSwapchainFormat;

    // Figure out how many images to use with the swapchain. Make sure we dont use more than the max.
    uint32_t preferredNumSwapChainImages = surfaceCapabilities.minImageCount + 1;
    uint32_t actualNumSwapchainImages    = (preferredNumSwapChainImages <= surfaceCapabilities.maxImageCount) ? 
                                                                                  preferredNumSwapChainImages : surfaceCapabilities.maxImageCount;

    // Choose a surface transform
    VkSurfaceTransformFlagBitsKHR preferredSurfaceTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    VkSurfaceTransformFlagBitsKHR preTransform              = ((surfaceCapabilities.supportedTransforms & preferredSurfaceTransform) > 0) ? 
                                                                                  preferredSurfaceTransform : surfaceCapabilities.currentTransform;

    VkCompositeAlphaFlagBitsKHR preferredCompositeBit = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    assert((surfaceCapabilities.supportedCompositeAlpha & preferredCompositeBit) != 0);

    /// The present mode is basically what determines vsync behavior. Whether there is or is not tearing, etc...
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    VkSwapchainCreateInfoKHR swapchainCreateInfo =
    {
        /*....VkStructureType..................sType...................*/ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        /*....const.void*......................pNext...................*/ 0,
        /*....VkSwapchainCreateFlagsKHR........flags...................*/ 0,
        /*....VkSurfaceKHR.....................surface.................*/ surface,
        /*....uint32_t.........................minImageCount...........*/ preferredNumSwapChainImages,
        /*....VkFormat.........................imageFormat.............*/ chosenSwapchainFormat.format,
        /*....VkColorSpaceKHR..................imageColorSpace.........*/ chosenSwapchainFormat.colorSpace,
        /*....VkExtent2D.......................imageExtent.............*/ swapchainExtent,
        /*....uint32_t.........................imageArrayLayers........*/ 1,
        /*....VkImageUsageFlags................imageUsage..............*/ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        /*....VkSharingMode....................imageSharingMode........*/ VK_SHARING_MODE_EXCLUSIVE,
        /*....uint32_t.........................queueFamilyIndexCount...*/ 0, //SPEC: queueFamilyIndexCount is the number of queue families having access to the image(s) of the swapchain when imageSharingMode is VK_SHARING_MODE_CONCURRENT.
        /*....const.uint32_t*..................pQueueFamilyIndices.....*/ 0,
        /*....VkSurfaceTransformFlagBitsKHR....preTransform............*/ preTransform,
        /*....VkCompositeAlphaFlagBitsKHR......compositeAlpha..........*/ preferredCompositeBit,
        /*....VkPresentModeKHR.................presentMode.............*/ swapchainPresentMode,
        /*....VkBool32.........................clipped.................*/ true,
        /*....VkSwapchainKHR...................oldSwapchain............*/ oldSwapchain
    };

    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    result = vkCreateSwapchainKHR(logicalDevice, &swapchainCreateInfo, 0, &swapchain);
    assert((result == VK_SUCCESS) && (swapchain != VK_NULL_HANDLE));

    //Get swapchain images
    uint32_t imageCount       = GetSwapChainImageCount(logicalDevice, swapchain);
    VkImage* pSwapchainImages = static_cast<VkImage*>(calloc(imageCount, sizeof(VkImage)));
    result = vkGetSwapchainImagesKHR(logicalDevice, swapchain, &imageCount, pSwapchainImages);
    assert(result == VK_SUCCESS);

    PerSwapchainImageResources* pPerSwapchainImageResources = *ppPerSwapchainImageResourcesInOut;

    if (pPerSwapchainImageResources != nullptr)
    {
        ResestPerSwapchainImageResources (logicalDevice, pPerSwapchainImageResources, *pNumSwapchainImages);
    }

    if (*pNumSwapchainImages != imageCount)
    {
        free (pPerSwapchainImageResources);
        pPerSwapchainImageResources = static_cast<PerSwapchainImageResources*>(calloc(imageCount, sizeof(PerSwapchainImageResources)));
    }

    // Create fence, create command buffer, and call vkAllocateCommandBuffers() - once for each swapchain image.
    for (uint32_t i = 0; i < imageCount; i++)
    {
        pPerSwapchainImageResources[i].queue_index = graphicsQueueIndex;
        //@NOTE: the semaphore in per frame resources arent initialized here.

        VkFenceCreateInfo fenceCreateInfo =
        {
            /*..VkStructureType.......sType.........*/ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            /*..const.void*...........pNext.........*/ 0,
            /*..VkFenceCreateFlags....flags.........*/ VK_FENCE_CREATE_SIGNALED_BIT
        };

        result = vkCreateFence(logicalDevice, &fenceCreateInfo, 0, &(pPerSwapchainImageResources[i].queueSubmitFence));
        assert(result == VK_SUCCESS);

#ifdef DEBUG
        printf("Created fence(0x%016x) for swapchain image #%u, initial status is %s\n", 
                       pPerSwapchainImageResources[i].queueSubmitFence,
                       i,
                       VkResultToString (vkGetFenceStatus (logicalDevice, pPerSwapchainImageResources[i].queueSubmitFence)));
#endif
        /// @TODO: make command buffer re-usable. There is no camera control in this app yet, so the selection of the geometry being drawn from frame to frame will not be changing.
        //              This allows transforms of individual meshes can be controled via model matrices in an ssbo without re-recording the command buffer. 
        VkCommandPoolCreateInfo commandPoolCreateInfo =
        {
            /*..VkStructureType.............sType.................*/ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            /*..const.void*.................pNext.................*/ 0,
            /*..VkCommandPoolCreateFlags....flags.................*/ VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            /*..uint32_t....................queueFamilyIndex......*/ graphicsQueueIndex
        };

        result = vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, 0, &(pPerSwapchainImageResources[i].commandPool));
        assert(result == VK_SUCCESS);


        VkCommandBufferAllocateInfo commandBufferAllocateInfo =
        {
            /*..VkStructureType.........sType................*/ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            /*..const.void*.............pNext................*/ 0,
            /*..VkCommandPool...........commandPool..........*/ pPerSwapchainImageResources[i].commandPool,
            /*..VkCommandBufferLevel....level................*/ VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            /*..uint32_t................commandBufferCount...*/ 1
        };

        // Allocate a command buffer so that it can be used later be used to present this image to screen.
        result = vkAllocateCommandBuffers( /*...VkDevice...........................device...........*/ logicalDevice,
                                           /*...const.VkCommandBufferAllocateInfo*.pAllocateInfo....*/ &commandBufferAllocateInfo,
                                           /*...VkCommandBuffer*...................pCommandBuffers..*/ &(pPerSwapchainImageResources[i].commandBuffer));
    }

    // Create image views for each of the present images we got from the swapchain
    for (uint32_t swapIdx = 0; swapIdx < imageCount; swapIdx++)
    {
        //I think the code flows better with this struct inisde the loop.Im assuming the compiler will optimize out the initialization after first iteration. 
        VkImageSubresourceRange subResourceRange =
        {
            /*...VkImageAspectFlags....aspectMask.........*/ VK_IMAGE_ASPECT_COLOR_BIT,
            /*...uint32_t..............baseMipLevel.......*/ 0,
            /*...uint32_t..............levelCount.........*/ 1,
            /*...uint32_t..............baseArrayLayer.....*/ 0,
            /*...uint32_t..............layerCount.........*/ 1
        };

#ifdef DEBUG
        assert (pSwapchainImages[swapIdx] != VK_NULL_HANDLE);
#endif

        VkImageViewCreateInfo imageViewCreateInfo =
        {
            /*...VkStructureType............sType...............*/ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            /*...const.void*................pNext...............*/ 0,
            /*...VkImageViewCreateFlags.....flags...............*/ 0,
            /*...VkImage....................image...............*/ pSwapchainImages[swapIdx],
            /*...VkImageViewType............viewType............*/ VK_IMAGE_VIEW_TYPE_2D,
            /*...VkFormat...................format..............*/ chosenSwapchainFormat.format,
            /*...VkComponentMapping.........components..........*/ { VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            /*...VkImageSubresourceRange....subresourceRange....*/ subResourceRange
        };
        result = vkCreateImageView(logicalDevice, &imageViewCreateInfo, 0, &(pPerSwapchainImageResources[swapIdx].presentImageViewHandle));
        assert(result == VK_SUCCESS);
    }

    // Choose a depth format from the preferred formats array arg
    VkFormat depthFormat = ChooseDepthFormat (physicalDevice,
                                              numPreferredDepthFormats,
                                              pPreferredDepthFormats);

    // Creates and allocates the iamges that will back the color, depth and resolve attachments
    for (uint32_t swapIdx = 0; swapIdx < imageCount; swapIdx++)
    {
        VkImage*         pDepthImage             = &(pPerSwapchainImageResources[swapIdx].depthImageHandle);
        VkDeviceMemory*  pDepthImageMemoryHandle = &(pPerSwapchainImageResources[swapIdx].depthImageMemoryHandle);
        VkImageView*     pDepthImageViewHandle   = &(pPerSwapchainImageResources[swapIdx].depthImageViewHandle);

        CreateAndAllocateDepthImage (logicalDevice,
                                     physicalDevice,
                                     graphicsQueueIndex,
                                     depthFormat,
                                     { swapchainExtent.width, swapchainExtent.height },
                                     pDepthImageMemoryHandle,
                                     pDepthImage,
                                     pDepthImageViewHandle,
                                     SceneVulkanParameters::depthAttachmentSamples);

        // Create image that will contain the unshaded fragment diffuse color
        VkImage*        pDiffuseImage             = &( pPerSwapchainImageResources[swapIdx].diffuseImageHandle       );
        VkDeviceMemory* pDiffuseImageMem          = &( pPerSwapchainImageResources[swapIdx].diffuseImageMemoryHandle );
        VkImageView*    pDiffuseImageView         = &( pPerSwapchainImageResources[swapIdx].diffuseImageViewHandle   );
        VkFormat        diffuseColorGbufferFormat = SceneVulkanParameters::diffuseColorGbufferImageFormat;

        CreateAndAllocateColorImage (logicalDevice,
                                     physicalDevice,
                                     graphicsQueueIndex,
                                     diffuseColorGbufferFormat,
                                     { swapchainExtent.width, swapchainExtent.height },
                                     pDiffuseImageMem,
                                     pDiffuseImage,
                                     pDiffuseImageView,
                                     SceneVulkanParameters::colorAttachmentSamples,
                                     VK_FALSE); // cant be used as an input attachment

        // Create image that will contain surface normal vectors
        VkImage*        pNormalImage               = &( pPerSwapchainImageResources[swapIdx].normalsImageHandle       );
        VkDeviceMemory* pNormalImageMem            = &( pPerSwapchainImageResources[swapIdx].normalsImageMemoryHandle );
        VkImageView*    pNormalImageView           = &( pPerSwapchainImageResources[swapIdx].normalsImageViewHandle   );
        VkFormat        surfaceNormalGbufferFormat = SceneVulkanParameters::normalVectorGbufferImageFormat;

        CreateAndAllocateColorImage (logicalDevice,
                                     physicalDevice,
                                     graphicsQueueIndex,
                                     surfaceNormalGbufferFormat,
                                     { swapchainExtent.width, swapchainExtent.height },
                                     pNormalImageMem,
                                     pNormalImage,
                                     pNormalImageView,
                                     SceneVulkanParameters::colorAttachmentSamples,
                                     VK_FALSE); // cant be used as an input attachment

        // Create image that will contain sample world position locations
        VkImage*        pPositionImage               = &( pPerSwapchainImageResources[swapIdx].positionImageHandle       );
        VkDeviceMemory* pPositionImageMem            = &( pPerSwapchainImageResources[swapIdx].positionImageMemoryHandle );
        VkImageView*    pPositionImageView           = &( pPerSwapchainImageResources[swapIdx].positionImageViewHandle   );
        VkFormat        surfacePositionGbufferFormat = SceneVulkanParameters::positionVectorGbufferImageFormat;

        CreateAndAllocateColorImage (logicalDevice,
                                     physicalDevice,
                                     graphicsQueueIndex,
                                     surfacePositionGbufferFormat,
                                     { swapchainExtent.width, swapchainExtent.height },
                                     pPositionImageMem,
                                     pPositionImage,
                                     pPositionImageView,
                                     SceneVulkanParameters::colorAttachmentSamples,
                                     VK_FALSE); // cant be used as an input attachment

        VkFormat resolveFormats[3] = { diffuseColorGbufferFormat, surfaceNormalGbufferFormat, surfacePositionGbufferFormat };

        // Create single sampled  resolve images
        for (uint32_t r = 0; r < SceneVulkanParameters::Subpass0::numSubpassResolveAttachments; r++)
        {
            CreateAndAllocateColorImage (logicalDevice,
                                         physicalDevice,
                                         graphicsQueueIndex,
                                         resolveFormats[r],
                                         { swapchainExtent.width, swapchainExtent.height },
                                         &(pPerSwapchainImageResources[swapIdx].pResolveAttachmentsMem[r]),
                                         &(pPerSwapchainImageResources[swapIdx].pResolveImageHandles[r]  ),
                                         &(pPerSwapchainImageResources[swapIdx].pResolveImageViews[r]    ),
                                         SceneVulkanParameters::resolveAttachmentSamples,
                                         VK_TRUE); // CAN be used as an input attachment

        }


        // Initialize with the size of the struct we want to update
        VkDeviceSize updateStagingBufferDataSize = sizeof (UniformBufferData);

        //Create a staging buffer which will be where the cpu writes matrix data to
        vulkanAllocatedBufferInfo updateStagingBufferInfo = CreateAndAllocaStagingBuffer (physicalDevice,
                                                                                          logicalDevice,
                                                                                          updateStagingBufferDataSize,
                                                                                          graphicsQueueIndex);

        // Create the staging buffer that will be used to update resources as needed
        pPerSwapchainImageResources[swapIdx].bufferUpdatesStagingBuffer = updateStagingBufferInfo.bufferHandle;
        pPerSwapchainImageResources[swapIdx].bufferUpdatesStagingMemory = updateStagingBufferInfo.memoryHandle;
        pPerSwapchainImageResources[swapIdx].updatesStagingBufferSize   = updateStagingBufferInfo.buffersize;
    }

    *pChosenDepthFormatOut             = depthFormat;
    *pNumSwapchainImages               = imageCount;
    *pSwapchainOut                     = swapchain;
    *ppPerSwapchainImageResourcesInOut = pPerSwapchainImageResources;
}

VkSurfaceFormatKHR ChooseSwapchainFormat(VkSurfaceFormatKHR* pSupportedSwapchainSurfaceFormats,
                                         uint32_t            numSupportedFormats,
                                         const VkFormat*     pPreferredFormats,
                                         uint32_t            numPreferredFormats)
{

    uint32_t           chosenSurfaceFormatIdx = UINT32_MAX;
    VkSurfaceFormatKHR chosenFormat           = { VK_FORMAT_UNDEFINED , VkColorSpaceKHR(0) };

    uint32_t           formatIdx = 0;
    while ((formatIdx < numSupportedFormats) && (chosenFormat.format == VK_FORMAT_UNDEFINED))
    {
        for (uint32_t preferredFormatIdx = 0; preferredFormatIdx < numPreferredFormats; preferredFormatIdx++)
        {
            if (pSupportedSwapchainSurfaceFormats[formatIdx].format == pPreferredFormats[preferredFormatIdx])
            {
                chosenFormat = pSupportedSwapchainSurfaceFormats[formatIdx]; //this ensures the above while loop breaks;
                break;
            }
        }
        formatIdx++;
    }

    return chosenFormat;
}

VkRenderPass CreateRenderpass(VkFormat swapChainFormat,  VkFormat depthFormat,  VkDevice logicalDevice)
{
    VkAttachmentDescription presentAttachmentDescription =
    {
        /*..VkAttachmentDescriptionFlags....flags............*/ 0,
        /*..VkFormat........................format...........*/ swapChainFormat,
        /*..VkSampleCountFlagBits...........samples..........*/ SceneVulkanParameters::presentAttachmentSamples,
        /*..VkAttachmentLoadOp..............loadOp...........*/ VK_ATTACHMENT_LOAD_OP_CLEAR,
        /*..VkAttachmentStoreOp.............storeOp..........*/ VK_ATTACHMENT_STORE_OP_STORE,
        /*..VkAttachmentLoadOp..............stencilLoadOp....*/ VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // Not using stencil so dont care
        /*..VkAttachmentStoreOp.............stencilStoreOp...*/ VK_ATTACHMENT_STORE_OP_DONT_CARE, // Not using stencil so dont care
        /*..VkImageLayout...................initialLayout....*/ VK_IMAGE_LAYOUT_UNDEFINED,
        /*..VkImageLayout...................finalLayout......*/ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentDescription colorAttachmentDescription =
    {
        /*..VkAttachmentDescriptionFlags....flags............*/ 0,
        /*..VkFormat........................format...........*/ swapChainFormat,
        /*..VkSampleCountFlagBits...........samples..........*/ SceneVulkanParameters::colorAttachmentSamples, // VK_SAMPLE_COUNT_4_BIT
        /*..VkAttachmentLoadOp..............loadOp...........*/ VK_ATTACHMENT_LOAD_OP_CLEAR,
        /*..VkAttachmentStoreOp.............storeOp..........*/ VK_ATTACHMENT_STORE_OP_STORE,
        /*..VkAttachmentLoadOp..............stencilLoadOp....*/ VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // Not using stencil so dont care
        /*..VkAttachmentStoreOp.............stencilStoreOp...*/ VK_ATTACHMENT_STORE_OP_DONT_CARE, // Not using stencil so dont care
        /*..VkImageLayout...................initialLayout....*/ VK_IMAGE_LAYOUT_UNDEFINED,
        /*..VkImageLayout...................finalLayout......*/ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentDescription normalAttachmentDescription =
    {
        /*..VkAttachmentDescriptionFlags....flags............*/ 0,
        /*..VkFormat........................format...........*/ SceneVulkanParameters::normalVectorGbufferImageFormat,
        /*..VkSampleCountFlagBits...........samples..........*/ SceneVulkanParameters::colorAttachmentSamples,  // VK_SAMPLE_COUNT_4_BIT
        /*..VkAttachmentLoadOp..............loadOp...........*/ VK_ATTACHMENT_LOAD_OP_CLEAR,
        /*..VkAttachmentStoreOp.............storeOp..........*/ VK_ATTACHMENT_STORE_OP_STORE,
        /*..VkAttachmentLoadOp..............stencilLoadOp....*/ VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // Not using stencil so dont care
        /*..VkAttachmentStoreOp.............stencilStoreOp...*/ VK_ATTACHMENT_STORE_OP_DONT_CARE, // Not using stencil so dont care
        /*..VkImageLayout...................initialLayout....*/ VK_IMAGE_LAYOUT_UNDEFINED,
        /*..VkImageLayout...................finalLayout......*/ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentDescription positionAttachmentDescription =
    {
        /*..VkAttachmentDescriptionFlags....flags............*/ 0,
        /*..VkFormat........................format...........*/ SceneVulkanParameters::positionVectorGbufferImageFormat,
        /*..VkSampleCountFlagBits...........samples..........*/ SceneVulkanParameters::colorAttachmentSamples,  // VK_SAMPLE_COUNT_4_BIT
        /*..VkAttachmentLoadOp..............loadOp...........*/ VK_ATTACHMENT_LOAD_OP_CLEAR,
        /*..VkAttachmentStoreOp.............storeOp..........*/ VK_ATTACHMENT_STORE_OP_STORE,
        /*..VkAttachmentLoadOp..............stencilLoadOp....*/ VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // Not using stencil so dont care
        /*..VkAttachmentStoreOp.............stencilStoreOp...*/ VK_ATTACHMENT_STORE_OP_DONT_CARE, // Not using stencil so dont care
        /*..VkImageLayout...................initialLayout....*/ VK_IMAGE_LAYOUT_UNDEFINED,
        /*..VkImageLayout...................finalLayout......*/ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentDescription depthAttachmentDescription =
    {
        /*..VkAttachmentDescriptionFlags....flags............*/ 0,
        /*..VkFormat........................format...........*/ depthFormat,
        /*..VkSampleCountFlagBits...........samples..........*/ SceneVulkanParameters::depthAttachmentSamples,
        /*..VkAttachmentLoadOp..............loadOp...........*/ VK_ATTACHMENT_LOAD_OP_CLEAR,
        /*..VkAttachmentStoreOp.............storeOp..........*/ VK_ATTACHMENT_STORE_OP_STORE,
        /*..VkAttachmentLoadOp..............stencilLoadOp....*/ VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // Not using stencil so dont care
        /*..VkAttachmentStoreOp.............stencilStoreOp...*/ VK_ATTACHMENT_STORE_OP_DONT_CARE, // Not using stencil so dont care
        /*..VkImageLayout.. ................initialLayout....*/ VK_IMAGE_LAYOUT_UNDEFINED,
        /*..VkImageLayout...................finalLayout......*/ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentDescription pColorResolveAttachments[SceneVulkanParameters::Subpass0::numSubpassColorAttachments] = {};

    VkFormat resolveFormats[] = { swapChainFormat,
                                  SceneVulkanParameters::normalVectorGbufferImageFormat,
                                  SceneVulkanParameters::positionVectorGbufferImageFormat };

#if DEBUG
    assert ((sizeof (resolveFormats) / sizeof (VkFormat)) == SceneVulkanParameters::Subpass0::numSubpassResolveAttachments);
#endif
                                  

    for (uint32_t i = 0; i < SceneVulkanParameters::Subpass0::numSubpassResolveAttachments; i++)
    {
        pColorResolveAttachments[i] =
        {
            /*..VkAttachmentDescriptionFlags....flags............*/ 0,
            /*..VkFormat........................format...........*/ resolveFormats[i],
            /*..VkSampleCountFlagBits...........samples..........*/ SceneVulkanParameters::resolveAttachmentSamples, // Resolve attachment gets the average of the 4 samples from a corresponding color attachment
            /*..VkAttachmentLoadOp..............loadOp...........*/ VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            /*..VkAttachmentStoreOp.............storeOp..........*/ VK_ATTACHMENT_STORE_OP_DONT_CARE, // the gbuffer images arent used outside of the renderpass, so this store op doesnt matter since the store op occurs at the end of the renderpass.
            /*..VkAttachmentLoadOp..............stencilLoadOp....*/ VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // Not using stencil so dont care
            /*..VkAttachmentStoreOp.............stencilStoreOp...*/ VK_ATTACHMENT_STORE_OP_DONT_CARE, // Not using stencil so dont care
            /*..VkImageLayout...................initialLayout....*/ VK_IMAGE_LAYOUT_UNDEFINED,
            /*..VkImageLayout...................finalLayout......*/ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };
    }

    VkAttachmentReference colorPresentAttachmentRef =
    {
        /*...uint32_t.........attachment....*/ SceneVulkanParameters::RenderPassParameters::swapchainColorAttachmentIndex, // 0
        /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference pSubpass0ColorAttachments[SceneVulkanParameters::Subpass0::numSubpassColorAttachments] =
    {
        {
            /*...uint32_t.........attachment....*/ SceneVulkanParameters::RenderPassParameters::diffuseColorAttachmentIndex, // 1
            /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        },
        {
            /*...uint32_t.........attachment....*/ SceneVulkanParameters::RenderPassParameters::surfaceNormalAttachmentIndex, // 2
            /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        },
        {
            /*...uint32_t.........attachment....*/ SceneVulkanParameters::RenderPassParameters::positionAttachmentIndex, // 3
            /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        }
    };

    VkAttachmentReference depthAttachmentRef =
    {
        /*...uint32_t.........attachment....*/ SceneVulkanParameters::RenderPassParameters::depthStencilAttachmentIndex, // 4
        /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference pSubpass0ColorResolveAttachments[SceneVulkanParameters::Subpass0::numSubpassColorAttachments] =
    {
        {
            /*...uint32_t.........attachment....*/ SceneVulkanParameters::RenderPassParameters::colorResolveAttachmentIndex, // 5
            /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        },
        {
            /*...uint32_t.........attachment....*/ SceneVulkanParameters::RenderPassParameters::normalResolveAttachmentIndex, // 6
            /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        },
        {
            /*...uint32_t.........attachment....*/ SceneVulkanParameters::RenderPassParameters::positionResolveAttachmentIndex,// 7
            /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        }
    };

    VkAttachmentReference pSubpass1InputAttachments[SceneVulkanParameters::Subpass1::numInputAttachments] =
    { 
        {
            /*...uint32_t.........attachment....*/ SceneVulkanParameters::RenderPassParameters::colorResolveAttachmentIndex, //5
            /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        },
        {
            /*...uint32_t.........attachment....*/ SceneVulkanParameters::RenderPassParameters::normalResolveAttachmentIndex, // 6
            /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        },
        {
            /*...uint32_t.........attachment....*/ SceneVulkanParameters::RenderPassParameters::positionResolveAttachmentIndex, // 7
            /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        }
    };



    VkSubpassDescription pSubpassDescriptions[SceneVulkanParameters::RenderPassParameters::numSubpasses] =
    {
        {//----SUBPASS 0----//
            /*...VkSubpassDescriptionFlags.......flags.....................*/ 0,
            /*...VkPipelineBindPoint.............pipelineBindPoint.........*/ VK_PIPELINE_BIND_POINT_GRAPHICS,
            /*...uint32_t........................inputAttachmentCount......*/ 0,
            /*...const.VkAttachmentReference*....pInputAttachments.........*/ nullptr,
            /*...uint32_t........................colorAttachmentCount......*/ SceneVulkanParameters::Subpass0::numSubpassColorAttachments, // 3
            /*...const.VkAttachmentReference*....pColorAttachments.........*/ pSubpass0ColorAttachments,
            /*...const.VkAttachmentReference*....pResolveAttachments.......*/ pSubpass0ColorResolveAttachments,
            /*...const.VkAttachmentReference*....pDepthStencilAttachment...*/ &depthAttachmentRef,
            /*...uint32_t........................preserveAttachmentCount...*/ 0,
            /*...const.uint32_t*.................pPreserveAttachments......*/ nullptr
        },
        {//----SUBPASS 1----//
            /*...VkSubpassDescriptionFlags.......flags.....................*/ 0,
            /*...VkPipelineBindPoint.............pipelineBindPoint.........*/ VK_PIPELINE_BIND_POINT_GRAPHICS,
            /*...uint32_t........................inputAttachmentCount......*/ SceneVulkanParameters::Subpass1::numInputAttachments, // 3 position, normal, color
            /*...const.VkAttachmentReference*....pInputAttachments.........*/ pSubpass1InputAttachments,
            /*...uint32_t........................colorAttachmentCount......*/ 1,
            /*...const.VkAttachmentReference*....pColorAttachments.........*/ &colorPresentAttachmentRef,
            /*...const.VkAttachmentReference*....pResolveAttachments.......*/ nullptr,
            /*...const.VkAttachmentReference*....pDepthStencilAttachment...*/ nullptr, // Not writing depth in second subpass
            /*...uint32_t........................preserveAttachmentCount...*/ 0,
            /*...const.uint32_t*.................pPreserveAttachments......*/ nullptr
        }
    };

    VkSubpassDependency   subpassDependencies[] =
    {
        { // This dependency ensures that previous reads and writes of the color attachments have completed, and are both availabl/visible before subpass 0
            /*....uint32_t................srcSubpass..........*/ VK_SUBPASS_EXTERNAL,
            /*....uint32_t................dstSubpass..........*/ 0,
            /*....VkPipelineStageFlags....srcStageMask........*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            /*....VkPipelineStageFlags....dstStageMask........*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            /*....VkAccessFlags...........srcAccessMask.......*/ VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            /*....VkAccessFlags...........dstAccessMask.......*/ VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            /*....VkDependencyFlags.......dependencyFlags.....*/ VK_DEPENDENCY_BY_REGION_BIT
        },
        { // This dependency ensures that previous reads and writes of the depth attachment have completed, and are both availabl/visible before subpass 0
            /*....uint32_t................srcSubpass..........*/ VK_SUBPASS_EXTERNAL,
            /*....uint32_t................dstSubpass..........*/ 0,
            /*....VkPipelineStageFlags....srcStageMask........*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            /*....VkPipelineStageFlags....dstStageMask........*/ VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            /*....VkAccessFlags...........srcAccessMask.......*/ VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            /*....VkAccessFlags...........dstAccessMask.......*/ VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
            /*....VkDependencyFlags.......dependencyFlags.....*/ VK_DEPENDENCY_BY_REGION_BIT
        },
        { // This dependency ensures resolves to the single sample attachments in subpass 0 have completed before the single sampled attachments are read as input attachments in subpass 1
            /*....uint32_t................srcSubpass..........*/ 0,
            /*....uint32_t................dstSubpass..........*/ 1,
            /*....VkPipelineStageFlags....srcStageMask........*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            /*....VkPipelineStageFlags....dstStageMask........*/ VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,// | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            /*....VkAccessFlags...........srcAccessMask.......*/ VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            /*....VkAccessFlags...........dstAccessMask.......*/ VK_ACCESS_INPUT_ATTACHMENT_READ_BIT,
            /*....VkDependencyFlags.......dependencyFlags.....*/ VK_DEPENDENCY_BY_REGION_BIT
        }
    };
    static const uint32_t numDependencies = sizeof(subpassDependencies)/sizeof(VkSubpassDependency);

    // Renderpass attachment descriptions.
    VkAttachmentDescription pAttachmentDescriptions[SceneVulkanParameters::RenderPassParameters::totalNumAttachments] =
    {
        presentAttachmentDescription,
        colorAttachmentDescription,
        normalAttachmentDescription,
        positionAttachmentDescription,
        depthAttachmentDescription,
        pColorResolveAttachments[0],
        pColorResolveAttachments[1],
        pColorResolveAttachments[2],
    };

#ifdef DEBUG
    // Make sure the number of members being initialized above matches VkAttachmentDescription pAttachmentDescriptions, ensuring no empty descriptions are part of the array.
    for (uint32_t i = 0; i < SceneVulkanParameters::RenderPassParameters::totalNumAttachments; i++)
    {
        assert (pAttachmentDescriptions[i].samples != 0);
    }

    // Making sure the attachment descriptions are at the correct index:
    assert (numDependencies == sizeof (subpassDependencies) / sizeof (VkSubpassDependency);
    assert (memcmp (&pAttachmentDescriptions[swapchainColorAttachmentIndex], &presentAttachmentDescription,  sizeof (VkAttachmentDescription)) == 0);
    assert (memcmp (&pAttachmentDescriptions[diffuseColorAttachmentIndex  ], &colorAttachmentDescription,    sizeof (VkAttachmentDescription)) == 0);
    assert (memcmp (&pAttachmentDescriptions[surfaceNormalAttachmentIndex ], &normalAttachmentDescription,   sizeof (VkAttachmentDescription)) == 0);
    assert (memcmp (&pAttachmentDescriptions[positionVectorAttachmentIndex], &positionAttachmentDescription, sizeof (VkAttachmentDescription)) == 0);
    assert (memcmp (&pAttachmentDescriptions[depthStencilAttachmentIndex  ], &depthAttachmentDescription,    sizeof (VkAttachmentDescription)) == 0);
#endif

    VkRenderPassCreateInfo renderPassCreateInfo =
    {
        /*...VkStructureType...................sType.............*/ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        /*...const.void*.......................pNext.............*/ 0,
        /*...VkRenderPassCreateFlags...........flags.............*/ 0,
        /*...uint32_t..........................attachmentCount...*/ SceneVulkanParameters::RenderPassParameters::totalNumAttachments,
        /*...const.VkAttachmentDescription*....pAttachments......*/ pAttachmentDescriptions,
        /*...uint32_t..........................subpassCount......*/ SceneVulkanParameters::RenderPassParameters::numSubpasses,
        /*...const.VkSubpassDescription*.......pSubpasses........*/ pSubpassDescriptions,
        /*...uint32_t..........................dependencyCount...*/ numDependencies,
        /*...const.VkSubpassDependency*........pDependencies.....*/ subpassDependencies
    };

    VkRenderPass renderpass = VK_NULL_HANDLE;
    VkResult     result     = vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, 0, &renderpass);

    assert(result == VK_SUCCESS);
    return renderpass;
}

struct AlignedAllocation
{
    uint8_t* pRawAllocatiotBaseAddr; // address returned from malloc/calloc
    uint8_t  alignment;              // requested alignment
    uint8_t* alignedBaseAddress;     // base address which satisfies the requested alignment, and which has alignedSize num bytes allocated starting there.
    size_t   alignedSize;            // size of allocation starting at alignedBaseAddress
};
/*
inline AlignedAllocation AlignedAlloc(uint32_t baseAddrAlignment,  // in bytes. Must be 1, 4, 8 or 16 for now
                                      uint32_t allocSize)
{
    assert((baseAddrAlignment == 1) ||
           (baseAddrAlignment == 4) ||
           (baseAddrAlignment == 8) ||
           (baseAddrAlignment == 16));

    assert(allocSize % baseAddrAlignment == 0);
    
    uint64_t rawAllocSize = static_cast<uint64_t>(allocSize) + static_cast<uint64_t>(baseAddrAlignment); //Adding alignment num bytes to the size ensures theres an aligned address with enough space
    void*    pAllocation  = calloc(rawAllocSize, 1);

    uint64_t  rawBaseAddr     = reinterpret_cast<uint64_t>(pAllocation);
    uint64_t  remainder       = rawBaseAddr % baseAddrAlignment; //this probably doesnt need to be uint64_t. probs could even just use an uint8_t
    uint64_t  alignedBaseAddr = rawBaseAddr + remainder;

    AlignedAllocation alignedAlloc      = {};
    alignedAlloc.pRawAllocationBaseAddr = static_cast<uint8_t*>(pAllocation);
    alignedAlloc.alignedBaseAddress     = reinterpret_cast<uint8_t*>(alignedBaseAddr);
    alignedAlloc.alignedSize            = (rawBaseAddr + rawAllocSize) - alignedBaseAddr; // subtract aligned base addr, from addr of last byte in allocation

    return alignedAlloc;
}*/

VkShaderModule CreateShaderModule(VkDevice    logicalDevice,
                                  const char* spvPath, // binary .spv file. likely produced via glslc
                                  bool        isFrag,
                                  bool        isVert)//@TODO: remove isFrag, since its not used,
{

    std::ifstream shaderSourceInputStream(spvPath, std::ifstream::binary | std::ios::ate); //std::ios::ate start At The End 
    assert(shaderSourceInputStream);

    std::streampos endPos     = shaderSourceInputStream.tellg();
    shaderSourceInputStream.seekg(0, std::ios::beg); //now at beginning
    size_t		   codeSize  = ( endPos - shaderSourceInputStream.tellg());
    assert(codeSize);
    uint32_t*      pCode     = static_cast<uint32_t*>(calloc(1, codeSize));
    assert(pCode);
    shaderSourceInputStream.read(reinterpret_cast<char*>(pCode), codeSize);

    VkShaderModule           shaderModule           = VK_NULL_HANDLE;
    VkShaderModuleCreateInfo shaderModuleCreateInfo =
    {
        /*...VkStructureType..............sType.......*/ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        /*...const.void*..................pNext.......*/ 0,
        /*...VkShaderModuleCreateFlags....flags.......*/ 0,
        /*...size_t.......................codeSize....*/ codeSize, // The validation layer seems to be using this as like how many ptrs ?
        /*...const.uint32_t*..............pCode.......*/ pCode// reinterpret_cast<uint32_t*>(spvCode.alignedBaseAddress)
    };

#ifdef DEBUG
    printf("codeSize = %llu, codesize / 4 = %llu\n", codeSize, codeSize / 4);
    printf("fromStruct: codeSize = %llu, codesize / 4 = %llu\n", shaderModuleCreateInfo.codeSize, shaderModuleCreateInfo.codeSize / 4);
#endif

    VkResult result = vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);
    assert(result == VK_SUCCESS);
    assert(shaderModule != VK_NULL_HANDLE);

    return shaderModule;
}

// Selectts a depth stencil format from the list provided to be used in the scene.
VkFormat ChooseDepthFormat (VkPhysicalDevice   physicalDeviceHandle,
                            uint32_t           numPrefferredDepthFormats,
                            const VkFormat*    pPreferredDepthFormats)
{
    // The feature bits we require for the depth attachment
    VkFormatFeatureFlags requiredFeatureFlags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
    VkFormat             chosenDepthFormat    = VK_FORMAT_UNDEFINED;

    for (uint32_t i = 0; i < numPrefferredDepthFormats; i++)
    {
        VkFormatProperties formatProperties = {};

        vkGetPhysicalDeviceFormatProperties (physicalDeviceHandle, pPreferredDepthFormats[i], &formatProperties);

        // Check for required format feature flag for optimal tiling
        if ((formatProperties.optimalTilingFeatures & requiredFeatureFlags) != 0)
        {
            chosenDepthFormat = pPreferredDepthFormats[i];
            break;
        }
    }

    if (chosenDepthFormat == VK_FORMAT_UNDEFINED)
    {
        printf ("Error: None of the requested depth formats are supported!\n");
        assert (false); //fault
    }

    return chosenDepthFormat;
}

// Function to create a single depth image.
void CreateAndAllocateDepthImage (VkDevice              logicalDeviceHandle,
                                  VkPhysicalDevice      physicalDeviceHandle,
                                  uint32_t              queueFamilyIdx,
                                  VkFormat              depthFormat,
                                  VkExtent2D            imageDimensions,
                                  VkDeviceMemory*       pDepthImageMemOut,
                                  VkImage*              pDepthImageHandleOut,
                                  VkImageView*          pDepthImageViewHandleOut,
                                  VkSampleCountFlagBits sampleCount)
{
    VkResult       result           = VK_INCOMPLETE;
    VkDeviceMemory depthImageMem    = VK_NULL_HANDLE;
    VkImage        depthImageHandle = VK_NULL_HANDLE;
    VkImageView    depthImageView   = VK_NULL_HANDLE;

    VkImageCreateInfo imageCreateInfo =
    {
        /*...VkStructureType..........sType...................*/ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        /*...const.void*..............pNext...................*/ nullptr,
        /*...VkImageCreateFlags.......flags...................*/ 0,
        /*...VkImageType..............imageType...............*/ VK_IMAGE_TYPE_2D,
        /*...VkFormat.................format..................*/ depthFormat,
        /*...VkExtent3D...............extent..................*/ {imageDimensions.width, imageDimensions.height, 1}, //@spec: "If imageType is VK_IMAGE_TYPE_1D, both extent.height and extent.depth must be 1"
        /*...uint32_t.................mipLevels...............*/ 1,
        /*...uint32_t.................arrayLayers.............*/ 1,
        /*...VkSampleCountFlagBits....samples.................*/ sampleCount, // if i want to use more than 1, i need to check the value the driver returns in VkPhysicalDeviceProperties::limits::framebufferDepthSampleCounts
        /*...VkImageTiling............tiling..................*/ VK_IMAGE_TILING_OPTIMAL,
        /*...VkImageUsageFlags........usage...................*/ VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        /*...VkSharingMode............sharingMode.............*/ VK_SHARING_MODE_EXCLUSIVE,
        /*...uint32_t.................queueFamilyIndexCount...*/ 1,
        /*...const.uint32_t*..........pQueueFamilyIndices.....*/ &queueFamilyIdx,
        /*...VkImageLayout............initialLayout...........*/ VK_IMAGE_LAYOUT_UNDEFINED
    };

    result = vkCreateImage (/*...VkDevice.........................device........*/ logicalDeviceHandle,
                            /*...const.VkImageCreateInfo*.........pCreateInfo...*/ &imageCreateInfo,
                            /*...const.VkAllocationCallbacks*.....pAllocator....*/ nullptr,
                            /*...VkImage*.........................pImage........*/ &depthImageHandle);
    assert (result == VK_SUCCESS);


    VkMemoryRequirements imageMemRequirements = {};
    vkGetImageMemoryRequirements (/*...VkDevice....................device................*/ logicalDeviceHandle,
                                  /*...VkImage.....................image.................*/ depthImageHandle,
                                  /*...VkMemoryRequirements*.......pMemoryRequirements...*/ &imageMemRequirements );

    uint32_t imagesMemTypeIndex = ChooseMemoryTypeIdx (physicalDeviceHandle,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                       &imageMemRequirements);
    assert (imagesMemTypeIndex != UINT32_MAX);

    VkMemoryAllocateInfo imageMemAllocateInfo =
    {
        /*...VkStructureType....sType.............*/ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        /*...const.void*........pNext.............*/ nullptr,
        /*...VkDeviceSize.......allocationSize....*/ imageMemRequirements.size,
        /*...uint32_t...........memoryTypeIndex...*/ imagesMemTypeIndex
    };

    // Allocate the memory that will back the depth image
    result = vkAllocateMemory (/*...VkDevice.......................device..........*/ logicalDeviceHandle,
                               /*...const.VkMemoryAllocateInfo*....pAllocateInfo...*/ &imageMemAllocateInfo,
                               /*...const.VkAllocationCallbacks*...pAllocator......*/ nullptr,
                               /*...VkDeviceMemory*................pMemory.........*/ &depthImageMem);
    assert (depthImageMem != VK_NULL_HANDLE);

    // Bind image memory to the depth image
    result = vkBindImageMemory (/*...VkDevice......................device..........*/ logicalDeviceHandle,
                                /*...VkImage.......................image...........*/ depthImageHandle,
                                /*...VkDeviceMemory................memory..........*/ depthImageMem,
                                /*...VkDeviceSize..................memoryOffset....*/ 0);
    assert (result == VK_SUCCESS);


    bool formatAlsoHasStencil = ((depthFormat == VK_FORMAT_D16_UNORM_S8_UINT) ||
                                 (depthFormat == VK_FORMAT_D24_UNORM_S8_UINT) ||
                                 (depthFormat == VK_FORMAT_D32_SFLOAT_S8_UINT)) ? true : false;

    bool formatIsDepthOnly = ((depthFormat == VK_FORMAT_X8_D24_UNORM_PACK32 ) ||
                              (depthFormat == VK_FORMAT_D16_UNORM           ) ||
                              (depthFormat == VK_FORMAT_D32_SFLOAT          )) ? true : false;

    VkImageSubresourceRange subResourceRange =
    {
        /*...VkImageAspectFlags....aspectMask.........*/ formatIsDepthOnly ? VkImageAspectFlags(VK_IMAGE_ASPECT_DEPTH_BIT) :
                                                                             VkImageAspectFlags(VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT),
        /*...uint32_t..............baseMipLevel.......*/ 0,
        /*...uint32_t..............levelCount.........*/ 1,
        /*...uint32_t..............baseArrayLayer.....*/ 0,
        /*...uint32_t..............layerCount.........*/ 1
    };

    VkImageViewCreateInfo imageViewCreateInfo =
    {
        /*...VkStructureType............sType...............*/ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        /*...const.void*................pNext...............*/ 0,
        /*...VkImageViewCreateFlags.....flags...............*/ 0,
        /*...VkImage....................image...............*/ depthImageHandle,
        /*...VkImageViewType............viewType............*/ VK_IMAGE_VIEW_TYPE_2D,
        /*...VkFormat...................format..............*/ depthFormat,
        /*...VkComponentMapping.........components..........*/ {},//{ VK_COMPONENT_SWIZZLE_IDENTITY },// { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
        /*...VkImageSubresourceRange....subresourceRange....*/ subResourceRange
    };
      
    result = vkCreateImageView (logicalDeviceHandle, &imageViewCreateInfo, 0, &depthImageView);


    *pDepthImageMemOut        = depthImageMem;
    *pDepthImageHandleOut     = depthImageHandle;
    *pDepthImageViewHandleOut = depthImageView;
}

// Creates and allocates memory for a color image of the specified resolution, format and sample count.
void CreateAndAllocateColorImage (VkDevice              logicalDeviceHandle,
                                  VkPhysicalDevice      physicalDeviceHandle,
                                  uint32_t              queueFamilyIdx,
                                  VkFormat              imageFormat,
                                  VkExtent2D            imageDimensions,
                                  VkDeviceMemory*       pColorImageMemOut,
                                  VkImage*              pColorImageHandleOut,
                                  VkImageView*          pColorImageViewHandleOut,
                                  VkSampleCountFlagBits sampleCount,
                                  VkBool32              canBeInputAttachment)
{
    VkResult       result           = VK_INCOMPLETE;
    VkDeviceMemory colorImageMem    = VK_NULL_HANDLE;
    VkImage        colorImageHandle = VK_NULL_HANDLE;
    VkImageView    colorImageView   = VK_NULL_HANDLE;

    VkImageCreateInfo imageCreateInfo =
    {
        /*...VkStructureType..........sType...................*/ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        /*...const.void*..............pNext...................*/ nullptr,
        /*...VkImageCreateFlags.......flags...................*/ 0,
        /*...VkImageType..............imageType...............*/ VK_IMAGE_TYPE_2D,
        /*...VkFormat.................format..................*/ imageFormat,
        /*...VkExtent3D...............extent..................*/ {imageDimensions.width, imageDimensions.height, 1}, //@spec: "If imageType is VK_IMAGE_TYPE_1D, both extent.height and extent.depth must be 1"
        /*...uint32_t.................mipLevels...............*/ 1,
        /*...uint32_t.................arrayLayers.............*/ 1,
        /*...VkSampleCountFlagBits....samples.................*/ sampleCount, // if i want to use more than 1, i need to check the value the driver returns in VkPhysicalDeviceProperties::limits::framebufferDepthSampleCounts
        /*...VkImageTiling............tiling..................*/ VK_IMAGE_TILING_OPTIMAL,
        /*...VkImageUsageFlags........usage...................*/ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        /*...VkSharingMode............sharingMode.............*/ VK_SHARING_MODE_EXCLUSIVE,
        /*...uint32_t.................queueFamilyIndexCount...*/ 1,
        /*...const.uint32_t*..........pQueueFamilyIndices.....*/ &queueFamilyIdx,
        /*...VkImageLayout............initialLayout...........*/ VK_IMAGE_LAYOUT_UNDEFINED
    };

    if (canBeInputAttachment == VK_TRUE)
    {
        imageCreateInfo.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    }

    result = vkCreateImage (/*...VkDevice.........................device........*/ logicalDeviceHandle,
                            /*...const.VkImageCreateInfo*.........pCreateInfo...*/ &imageCreateInfo,
                            /*...const.VkAllocationCallbacks*.....pAllocator....*/ nullptr,
                            /*...VkImage*.........................pImage........*/ &colorImageHandle);
    assert (result == VK_SUCCESS);

    VkMemoryRequirements imageMemRequirements = {};
    vkGetImageMemoryRequirements (/*...VkDevice....................device................*/ logicalDeviceHandle,
                                  /*...VkImage.....................image.................*/ colorImageHandle,
                                  /*...VkMemoryRequirements*.......pMemoryRequirements...*/ &imageMemRequirements);

    uint32_t imagesMemTypeIndex = ChooseMemoryTypeIdx (physicalDeviceHandle,
                                                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                                       &imageMemRequirements);
    assert (imagesMemTypeIndex != UINT32_MAX);

    VkMemoryAllocateInfo imageMemAllocateInfo =
    {
        /*...VkStructureType....sType.............*/ VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        /*...const.void*........pNext.............*/ nullptr,
        /*...VkDeviceSize.......allocationSize....*/ imageMemRequirements.size,
        /*...uint32_t...........memoryTypeIndex...*/ imagesMemTypeIndex
    };

    // Allocate the memory that will back the color image
    result = vkAllocateMemory (/*...VkDevice.......................device..........*/ logicalDeviceHandle,
                               /*...const.VkMemoryAllocateInfo*....pAllocateInfo...*/ &imageMemAllocateInfo,
                               /*...const.VkAllocationCallbacks*...pAllocator......*/ nullptr,
                               /*...VkDeviceMemory*................pMemory.........*/ &colorImageMem);
    assert (colorImageMem != VK_NULL_HANDLE);

    // Bind image memory to the color image
    result = vkBindImageMemory (/*...VkDevice......................device..........*/ logicalDeviceHandle,
                                /*...VkImage.......................image...........*/ colorImageHandle,
                                /*...VkDeviceMemory................memory..........*/ colorImageMem,
                                /*...VkDeviceSize..................memoryOffset....*/ 0);
    assert (result == VK_SUCCESS);


    VkImageSubresourceRange subResourceRange =
    {
        /*...VkImageAspectFlags....aspectMask.........*/ VK_IMAGE_ASPECT_COLOR_BIT,
        /*...uint32_t..............baseMipLevel.......*/ 0,
        /*...uint32_t..............levelCount.........*/ 1,
        /*...uint32_t..............baseArrayLayer.....*/ 0,
        /*...uint32_t..............layerCount.........*/ 1
    };

    VkImageViewCreateInfo imageViewCreateInfo =
    {
        /*...VkStructureType............sType...............*/ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        /*...const.void*................pNext...............*/ 0,
        /*...VkImageViewCreateFlags.....flags...............*/ 0,
        /*...VkImage....................image...............*/ colorImageHandle,
        /*...VkImageViewType............viewType............*/ VK_IMAGE_VIEW_TYPE_2D,
        /*...VkFormat...................format..............*/ imageFormat,
        /*...VkComponentMapping.........components..........*/ {VK_COMPONENT_SWIZZLE_IDENTITY},
        /*...VkImageSubresourceRange....subresourceRange....*/ subResourceRange
    };

    result = vkCreateImageView (logicalDeviceHandle, &imageViewCreateInfo, 0, &colorImageView);

    *pColorImageMemOut        = colorImageMem;
    *pColorImageHandleOut     = colorImageHandle;
    *pColorImageViewHandleOut = colorImageView;
}

void CreateFrameBuffers(VkDevice                    logicalDevice,
                        VkRenderPass                renderPass,
                        VkExtent2D*                 pExtent,
                        uint32_t                    numSwapchainImages,
                        PerSwapchainImageResources* pPerSwapchainImageResources)
{
    const uint32_t numColorAttachments   = SceneVulkanParameters::RenderPassParameters::numColorAttachments; // 4 - diffuse color + surface normals + position + present
    const uint32_t numDepthAttachments   = SceneVulkanParameters::RenderPassParameters::numDepthAttachments; // 1
    const uint32_t numResolveAttachments = SceneVulkanParameters::Subpass0::numSubpassColorAttachments;

    VkResult                result                  = VK_INCOMPLETE;
    static const uint32_t   numAttachments          = numColorAttachments +
                                                      numDepthAttachments +
                                                      numResolveAttachments;
    VkFramebufferCreateInfo framebufferCreateInfo   =
    {
        /*...VkStructureType.............sType.............*/ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        /*...const.void*.................pNext.............*/ nullptr, //@VKSPec: "pNext must be NULL or a pointer to a valid instance of VkFramebufferAttachmentsCreateInfo"
        /*...VkFramebufferCreateFlags....flags.............*/ 0,
        /*...VkRenderPass................renderPass........*/ renderPass,
        /*...uint32_t....................attachmentCount...*/ numAttachments,
        /*...const.VkImageView*..........pAttachments......*/ nullptr, // pAttachments is set from inside the loop below.
        /*...uint32_t....................width.............*/ pExtent->width,
        /*...uint32_t....................height............*/ pExtent->height,
        /*...uint32_t....................layers............*/ 1
    };

    assert(renderPass != VK_NULL_HANDLE);
     
    for (uint32_t swapIdx = 0; swapIdx < numSwapchainImages; swapIdx++)
    {
        PerSwapchainImageResources* pSwapImgResourceSet                   = &pPerSwapchainImageResources[swapIdx];
        VkImageView                 pAttachmentImageViews[numAttachments] = {};
        pAttachmentImageViews[SceneVulkanParameters::RenderPassParameters::swapchainColorAttachmentIndex] = pSwapImgResourceSet->presentImageViewHandle;
        pAttachmentImageViews[SceneVulkanParameters::RenderPassParameters::diffuseColorAttachmentIndex  ] = pSwapImgResourceSet->diffuseImageViewHandle;
        pAttachmentImageViews[SceneVulkanParameters::RenderPassParameters::surfaceNormalAttachmentIndex ] = pSwapImgResourceSet->normalsImageViewHandle;
        pAttachmentImageViews[SceneVulkanParameters::RenderPassParameters::positionAttachmentIndex      ] = pSwapImgResourceSet->positionImageViewHandle;
        pAttachmentImageViews[SceneVulkanParameters::RenderPassParameters::depthStencilAttachmentIndex  ] = pSwapImgResourceSet->depthImageViewHandle;

        // Attachments Used as Resolve attachments in subpass 0, and input attachments in subpass 1
        pAttachmentImageViews[SceneVulkanParameters::RenderPassParameters::colorResolveAttachmentIndex   ] = pSwapImgResourceSet->pResolveImageViews[0];
        pAttachmentImageViews[SceneVulkanParameters::RenderPassParameters::normalResolveAttachmentIndex  ] = pSwapImgResourceSet->pResolveImageViews[1];
        pAttachmentImageViews[SceneVulkanParameters::RenderPassParameters::positionResolveAttachmentIndex] = pSwapImgResourceSet->pResolveImageViews[2];

        framebufferCreateInfo.pAttachments = pAttachmentImageViews;

        VkFramebuffer* pFramebuffer = &(pSwapImgResourceSet->framebufferHandle);
        result = vkCreateFramebuffer(/*...VkDevice............................device........*/ logicalDevice,
                                     /*...const.VkFramebufferCreateInfo*......pCreateInfo...*/ &framebufferCreateInfo,
                                     /*...const.VkAllocationCallbacks*........pAllocator....*/ 0,
                                     /*...VkFramebuffer*......................pFramebuffer..*/ pFramebuffer);

        assert((result == VK_SUCCESS) && (*pFramebuffer != VK_NULL_HANDLE));
    }
}
//need to figure out why this is something that needs a swapchain image index...
void WaitOnPendingSwapchainImageFence(PerSwapchainImageResources* pPerFrameResources,
                                      VkDevice                    logicalDevice,
                                      uint32_t                    swapchainImageIdx)
{
    PerSwapchainImageResources* pSwapchainImageResources = &(pPerFrameResources[swapchainImageIdx]);
    assert (pSwapchainImageResources->queueSubmitFence != VK_NULL_HANDLE);

    VkResult fenceStatus = vkGetFenceStatus (logicalDevice, pSwapchainImageResources->queueSubmitFence);

    fenceStatus = vkWaitForFences (logicalDevice, 1, &(pSwapchainImageResources->queueSubmitFence), VK_TRUE, UINT64_MAX);
    assert (fenceStatus == VK_SUCCESS);
    fenceStatus = vkResetFences (logicalDevice, 1, &(pSwapchainImageResources->queueSubmitFence));
    assert (fenceStatus == VK_SUCCESS);

    if (pSwapchainImageResources->commandPool != VK_NULL_HANDLE)
    { 
        vkResetCommandPool(logicalDevice, pSwapchainImageResources->commandPool, 0);
    }
    else { assert (0); } // Im expecting the else to never be hit.
}

VkResult AcuireNextSwapchainImageIdx(VkQueue                     queue,
                                     VkDevice                    logicalDevice,
                                     VkSwapchainKHR              swapchain,
                                     uint32_t*                   pSwapchainImageIdxOut,
                                     PerSwapchainImageResources* pSwapchainImageResources)
{
    
    VkSemaphore nextFrameImageAcquireSemaphore = VkSync::SemaphoreDepot.ObtainSemaphore();
    VkResult    result                         = VK_INCOMPLETE;

    ///@Note: Acquire the index of the next swapchain image that we will render to.
    ///       There is a chance the image is still being used from the last time it was used though.
    ///       So later we need to make sure we tell the GPU that any work we submit involving this 
    ///       swapchain image cant start until nextFrameImageAcquireSemaphore has been signaled.
    ///       Thats done by making sure nextFrameImageAcquireSemaphore is listed in
    ///       the VkSubmitInfo::pWaitSemaphores array when we submit the work for execution.
    /// 
    result = vkAcquireNextImageKHR(/*..VkDevice.........device.......*/ logicalDevice,
                                   /*..VkSwapchainKHR...swapchain....*/ swapchain,
                                   /*..uint64_t.........timeout......*/ UINT64_MAX,
                                   /*..VkSemaphore......semaphore....*/ nextFrameImageAcquireSemaphore,
                                   /*..VkFence..........fence........*/ VK_NULL_HANDLE, //if semaphore isnt null, than this isnt needed
                                   /*..uint32_t*........pImageIndex..*/ pSwapchainImageIdxOut);

    assert (result == VK_SUCCESS);// Should be success since the timout param above is going to make it block indefinitely..,

    PerSwapchainImageResources* pSwapchainImageResourceSet    = &(pSwapchainImageResources[*pSwapchainImageIdxOut]);

    WaitOnPendingSwapchainImageFence(pSwapchainImageResources,
                                     logicalDevice,
                                     *pSwapchainImageIdxOut);


    // Wait for queue to be idle.
    if (result == VK_NOT_READY)
    {
        result = vkQueueWaitIdle (queue);
        assert (result == VK_SUCCESS);
    }

    //if this semaphore isnt a null handle, than it means its been used on the previous frame so not needed?
    VkSemaphore previousImageAcquireSemaphore = pSwapchainImageResourceSet->acquireSwapchainImageSemaphore;
    if (pSwapchainImageResourceSet->acquireSwapchainImageSemaphore != VK_NULL_HANDLE)
    {
        vkDestroySemaphore (logicalDevice, previousImageAcquireSemaphore, nullptr);
        pSwapchainImageResourceSet->acquireSwapchainImageSemaphore = VK_NULL_HANDLE;
        //VkSync::SemaphoreDepot.DepositUnusedSemaphore(pSwapchainImageResourceSet->acquireSwapchainImageSemaphore);
    }


    VkSemaphore previousImageReleaseSemaphore = pSwapchainImageResourceSet->releaseSwapchainImageSemaphore;
    if (previousImageReleaseSemaphore != VK_NULL_HANDLE)
    {
        vkDestroySemaphore (logicalDevice, previousImageReleaseSemaphore, nullptr);
        pSwapchainImageResourceSet->releaseSwapchainImageSemaphore = VK_NULL_HANDLE;
    }


    pSwapchainImageResourceSet->acquireSwapchainImageSemaphore = nextFrameImageAcquireSemaphore;
    

    return result;
}

uint64_t ExecuteRenderLoop (VkDevice                     logicalDevice,
                            VkPhysicalDevice             physicalDevice,
                            VkSwapchainKHR               swapchain,
                            VkQueue                      queue,
                            PerSubpassRenderParameters* pSubpass0Parameters,
                            PerSubpassRenderParameters* pSubpass1Parameters,
                            uint32_t                     gfxQueueIdx,
                            uint32_t                     numPreferredSwapchainFormats,
                            uint32_t                     numPreferredDepthFormats,
                            const VkFormat*              pPreferredSwapchainFormats,
                            const VkFormat*              pPreferredDepthFormats,
                            VkSurfaceKHR                 surface,
                            VkRenderPass                 renderpass,
                            PerSwapchainImageResources** ppPerSwapchainImageResources,
                            uint32_t*                    pNumSwapchainImages,
                            VkExtent2D*                  pExtent,
                            GeometryBufferSet*           pGeometryBufferSet,
                            UniformBufferData*           pUboData,
                            vulkanAllocatedBufferInfo*   pUniformBufferInfo)
{
    uint64_t                    time                = 0; ///@TODO: Look into any light-weight profiling measurements that can be taken here and returned from the function.
    VkResult                    result              = VK_INCOMPLETE;
    VkCommandBuffer             renderCommandBuffer = VK_NULL_HANDLE;

    uint32_t imageIdx;
    result = AcuireNextSwapchainImageIdx(queue, logicalDevice, swapchain, &imageIdx, *ppPerSwapchainImageResources);

    // This is expected to be hit when the window has resized, or when some other change to the rendering surface that requires reinitialization
    if ((result == VK_SUBOPTIMAL_KHR) || (result == VK_ERROR_OUT_OF_DATE_KHR))
    {
        printf ("Resizing window...\n");
        swapchain = ReinitializeRenderungSurface (/*...VkDevice.....................logicalDevice...................*/ logicalDevice,
                                                  /*...VkPhysicalDevice.............physicalDevice..................*/ physicalDevice,
                                                  /*...uint32_t.....................gfxQueueIndex...................*/ gfxQueueIdx,
                                                  /*...VkSwapchainKHR...............swapchain.......................*/ swapchain,
                                                  /*...VkExtent2D...................swapchainExtent.................*/ *pExtent,
                                                  /*...uint32_t.....................numPreferredDepthFormats........*/ numPreferredSwapchainFormats,
                                                  /*...uint32_t.....................numPreferredSwapchainFormats....*/ numPreferredDepthFormats,
                                                  /*...VkFormat*.....................pPreferredSwapchainFormats.....*/ pPreferredDepthFormats,
                                                  /*...VkFormat*.....................pPreferredDepthFormats.........*/ pPreferredDepthFormats,
                                                  /*...VkSurfaceKHR..................surface........................*/ surface,
                                                  /*...VkRenderPass..................renderpass.....................*/ renderpass,
                                                  /*...VkQueue.......................queue..........................*/ queue,
                                                  /*...uint32_t*.....................pNumSwapchainImages............*/ pNumSwapchainImages,
                                                  /*...PerSwapchainImageResources**..ppPerSwapchainImageResources...*/ ppPerSwapchainImageResources);

        result = AcuireNextSwapchainImageIdx(queue, logicalDevice, swapchain, &imageIdx, *ppPerSwapchainImageResources);
    }

    VkClearColorValue colorClearValue = { 0.01f, 0.033f, 0.01f , 1.0f };

    VkClearDepthStencilValue depthClearValue = {/*...float.......depth.....*/ 1.0f,
                                                /*...uint32_t....stencil...*/ 0 };
 
    PerSwapchainImageResources* pCurrentSwapImageResources = &((*ppPerSwapchainImageResources)[imageIdx]);

    //Now that we know which swapchain index we will be using we can fill in the rest of subpass1Params
    pSubpass1Parameters->descriptorSet = pCurrentSwapImageResources->subpass1DesciptorSetHandle;

    // Write ubo data to the ubo staging buffer
    vulkanAllocatedBufferInfo pUboUpdateStagingBufferInfo = {};
    pUboUpdateStagingBufferInfo.bufferHandle = pCurrentSwapImageResources->bufferUpdatesStagingBuffer;
    pUboUpdateStagingBufferInfo.buffersize   = pCurrentSwapImageResources->updatesStagingBufferSize;
    pUboUpdateStagingBufferInfo.memoryHandle = pCurrentSwapImageResources->bufferUpdatesStagingMemory;

    void* pMappedUboUpdateStagingBuff = MapBufferMemory (pUboUpdateStagingBufferInfo, logicalDevice);
    memcpy (pMappedUboUpdateStagingBuff, pUboData, sizeof (UniformBufferData));
    vkUnmapMemory (logicalDevice, pUboUpdateStagingBufferInfo.memoryHandle);

    //Write *pUboData to the staging buffer corresponding to this swapchain index
    // Than pass it to the functio below, where the command buffer will need to include a buffer copy. 

    renderCommandBuffer = RecordRenderGeometryBufferCmds (/*...GeometryBufferSet*...........pGeometryBufferSet............*/ pGeometryBufferSet,
                                                          /*...PerSwapchainImageResources*..pPerSwapchainImageResources...*/ pCurrentSwapImageResources,
                                                          /*...PerSubpassRenderParameters*..pSubpass0Parameters...........*/ pSubpass0Parameters,
                                                          /*...PerSubpassRenderParameters*..pSubpass1Parameters...........*/ pSubpass1Parameters,
                                                          /*...vulkanAllocatedBufferInfo*...pUniformBufferInfo............*/ pUniformBufferInfo,
                                                          /*...VkRenderPass.................renderPass....................*/ renderpass,
                                                          /*...VkExtent2D*..................pExtent.......................*/ pExtent,
                                                          /*...VkClearColorValue*...........pClearValue...................*/ &colorClearValue,
                                                          /*...VkClearDepthStencilValue*....pClearValue...................*/ &depthClearValue );

    result = SubmitRenderCommandBuffer (/*...VkCommandBuffer...............commandBuffer.................*/ renderCommandBuffer,
                                        /*...VkQueue.......................queue.........................*/ queue,
                                        /*...PerSwapchainImageResources*...pPerSwapchainImageResources...*/ pCurrentSwapImageResources);

    // present image
    result = PresentImage(swapchain, imageIdx, pCurrentSwapImageResources->releaseSwapchainImageSemaphore, queue);
   
    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        printf ("Present image returned %s. May not be able to recover.\n", (result == VK_SUBOPTIMAL_KHR) ? "VK_SUBOPTIMAL_KHR" : "VK_ERROR_OUT_OF_DATE_KHR");
        swapchain = ReinitializeRenderungSurface (/*...VkDevice.....................logicalDevice...................*/ logicalDevice,
                                                  /*...VkPhysicalDevice.............physicalDevice..................*/ physicalDevice,
                                                  /*...uint32_t.....................gfxQueueIndex...................*/ gfxQueueIdx,
                                                  /*...VkSwapchainKHR...............swapchain.......................*/ swapchain,
                                                  /*...VkExtent2D...................swapchainExtent.................*/ *pExtent,
                                                  /*...uint32_t.....................numPreferredDepthFormats........*/ numPreferredSwapchainFormats,
                                                  /*...uint32_t.....................numPreferredSwapchainFormats....*/ numPreferredDepthFormats,
                                                  /*...VkFormat*.....................pPreferredSwapchainFormats.....*/ pPreferredDepthFormats,
                                                  /*...VkFormat*.....................pPreferredDepthFormats.........*/ pPreferredDepthFormats,
                                                  /*...VkSurfaceKHR..................surface........................*/ surface,
                                                  /*...VkRenderPass..................renderpass.....................*/ renderpass,
                                                  /*...VkQueue.......................queue..........................*/ queue,
                                                  /*...uint32_t*.....................pNumSwapchainImages............*/ pNumSwapchainImages,
                                                  /*...PerSwapchainImageResources*...pPerSwapchainImageResources....*/ ppPerSwapchainImageResources);
    }
    else if (result != VK_SUCCESS)
    {
        printf ("Error presenting swapchain image\n");
        assert (0);
    }

    return time;
}

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
                                            PerSwapchainImageResources** ppPerSwapchainImageResources)
{
    assert(logicalDevice != VK_NULL_HANDLE);

    VkSwapchainKHR           newSwapChain        = VK_NULL_HANDLE;
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    VkResult                 result              = VK_INCOMPLETE;

    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

    //If window/surface resolution has changed
    if ((surfaceCapabilities.currentExtent.height != swapchainExtent.height) ||
        (surfaceCapabilities.currentExtent.width  != swapchainExtent.width))
    {
        vkDeviceWaitIdle(logicalDevice);

        //teardown_framebuffers
        vkQueueWaitIdle(queue);

        VkExtent2D                  newSwapChainExtent          = {};
        VkSurfaceFormatKHR          newSwapchainSurfaceFormat   = {};
        VkFormat                    chosenDepthFormat           = VK_FORMAT_UNDEFINED;
        InitializeSwapchain (/*...VkPhysicalDevice..............physicalDevice.................*/ physicalDevice,
                             /*...VkDevice......................logicalDevice..................*/ logicalDevice,
                             /*...uint32_t......................graphicsQueueIndex.............*/ gfxQueueIndex,
                             /*...VkSurfaceKHR..................surface........................*/ surface,
                             /*...uint32_t......................numPreferredSurfaceFormats.....*/ numPreferredSwapchainFormats,
                             /*...uint32_t......................numPreferredDepthFormats.......*/ numPreferredDepthFormats,
                             /*...const.VkFormat*...............pPreferredSurfaceFormats.......*/ pPreferredSwapchainFormats,
                             /*...const.VkFormat*...............pPreferredDepthFormats.........*/ pPreferredDepthFormats,
                             /*...VkExtent2D....................prefferredExtent...............*/ swapchainExtent,
                             /*...VkSwapchainKHR................oldSwapchain...................*/ swapchain,
                             /*...VkSwapchainKHR*...............pSwapchainOut..................*/ &newSwapChain,
                             /*...VkExtent2D*...................pSwapchainExtentChosenOut......*/ &newSwapChainExtent,
                             /*...VkSurfaceFormatKHR*...........pSurfaceFormatOut..............*/ &newSwapchainSurfaceFormat,
                             /*...VkFormat*.....................pChosenDepthFormatOut..........*/ &chosenDepthFormat,
                             /*...uint32_t*.....................pNumSwapchainImages............*/ pNumSwapchainImages,
                             /*...PerSwapchainImageResources**..ppPerSwapchainImageResources...*/ ppPerSwapchainImageResources);

        assert ((swapchainExtent.height == newSwapChainExtent.height) &&
                (swapchainExtent.width  == newSwapChainExtent.width ));

        CreateFrameBuffers(/*...VkDevice......................logicalDevice.................*/ logicalDevice,
                           /*...VkRenderPass..................renderPass....................*/ renderpass,
                           /*...VkExtent2D*...................pExtent.......................*/ &swapchainExtent,
                           /*...uint32_t......................numSwapchainImages............*/ *pNumSwapchainImages,
                           /*...PerSwapchainImageResources*...pPerSwapchainImageResources...*/ *ppPerSwapchainImageResources);
    }
    else
    {
        newSwapChain = swapchain; 
    }

    return newSwapChain;
}

VkCommandBuffer RecordRenderGeometryBufferCmds(GeometryBufferSet*          pGeometryBufferSet,
                                               PerSwapchainImageResources* pPerSwapchainImageResources,
                                               PerSubpassRenderParameters* pSubpass0Parameters,
                                               PerSubpassRenderParameters* pSubpass1Parameters,
                                               vulkanAllocatedBufferInfo*  pUniformBufferInfo,
                                               VkRenderPass                renderPass,
                                               VkExtent2D*                 pExtent,
                                               VkClearColorValue*          pColorClearValue,
                                               VkClearDepthStencilValue*   pDepthStencilClearValue)
{
    assert (pGeometryBufferSet      != nullptr);
    assert (pExtent                 != nullptr);
    assert (pColorClearValue        != nullptr);
    assert (pDepthStencilClearValue != nullptr);

    VkFramebuffer   framebuffer   = pPerSwapchainImageResources->framebufferHandle;
    VkCommandBuffer commandBuffer = pPerSwapchainImageResources->commandBuffer;

    VkCommandBufferBeginInfo cmdBufferBeginInfo =
    {
        /*...VkStructureType..........................sType..............*/ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        /*...const.void*..............................pNext..............*/ 0,
        /*...VkCommandBufferUsageFlags................flags..............*/ VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        /*...const.VkCommandBufferInheritanceInfo*....pInheritanceInfo...*/ 0
    };

    // Begin command buffer
    vkBeginCommandBuffer (commandBuffer, &cmdBufferBeginInfo);

    VkBufferCopy copyInfo =
    {
        /*...VkDeviceSize....srcOffset...*/ 0,
        /*...VkDeviceSize....dstOffset...*/ 0,
        /*...VkDeviceSize....size........*/ sizeof(UniformBufferData)
    };


    // Upload data from ubo staging buffer to the actual uniform buffer itself
    vkCmdCopyBuffer (/*...VkCommandBuffer.............................commandBuffer...*/ commandBuffer,
                     /*...VkBuffer....................................srcBuffer.......*/ pPerSwapchainImageResources->bufferUpdatesStagingBuffer,
                     /*...VkBuffer....................................dstBuffer.......*/ pUniformBufferInfo->bufferHandle,
                     /*...uint32_t....................................regionCount.....*/ 1,
                     /*...const.VkBufferCopy*.........................pRegions........*/ &copyInfo);

    VkRect2D renderArea =
    {
        /*...VkOffset2D...offset...*/ {0, 0},
        /*...VkExtent2D...extent...*/ *pExtent
    };

    // 2 clear values: one for color, one for depth.
    const uint32_t numClearValues               = SceneVulkanParameters::RenderPassParameters::totalNumAttachments;
    VkClearValue   pClearValues[numClearValues] = {};

    pClearValues[SceneVulkanParameters::RenderPassParameters::swapchainColorAttachmentIndex].color        = *pColorClearValue;
    pClearValues[SceneVulkanParameters::RenderPassParameters::diffuseColorAttachmentIndex  ].color        = *pColorClearValue;
    pClearValues[SceneVulkanParameters::RenderPassParameters::surfaceNormalAttachmentIndex ].color        = *pColorClearValue;
    pClearValues[SceneVulkanParameters::RenderPassParameters::positionAttachmentIndex      ].color        = *pColorClearValue;
    pClearValues[SceneVulkanParameters::RenderPassParameters::depthStencilAttachmentIndex  ].depthStencil = *pDepthStencilClearValue;

    VkRenderPassBeginInfo renderPassBeginInfo =
    {
        /*...VkStructureType........sType..............*/ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        /*...const.void*............pNext..............*/ 0,
        /*...VkRenderPass...........renderPass.........*/ renderPass,
        /*...VkFramebuffer..........framebuffer........*/ framebuffer,
        /*...VkRect2D...............renderArea.........*/ renderArea,
        /*...uint32_t...............clearValueCount....*/ numClearValues,
        /*...const.VkClearValue*....pClearValues.......*/ pClearValues
    };

    vkCmdBeginRenderPass (commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline (commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pSubpass0Parameters->pipeline);

    vkCmdBindDescriptorSets (/*...VkCommandBuffer..........commandBuffer.........*/ commandBuffer,
                             /*...VkPipelineBindPoint......pipelineBindPoint.....*/ VK_PIPELINE_BIND_POINT_GRAPHICS,
                             /*...VkPipelineLayout.........layout................*/ pSubpass0Parameters->pipelineLayout,
                             /*...uint32_t.................firstSet..............*/ 0,
                             /*...uint32_t.................descriptorSetCount....*/ 1,
                             /*...const.VkDescriptorSet*...pDescriptorSets.......*/ &(pSubpass0Parameters->descriptorSet),
                             /*...uint32_t.................dynamicOffsetCount....*/ 0,
                             /*...const.uint32_t*..........pDynamicOffsets.......*/ nullptr);

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

    VkDeviceSize offsets[] = { 0 };

    vkCmdBindVertexBuffers (/*..VkCommandBuffer........commandBuffer...*/ commandBuffer,
                            /*..uint32_t...............firstBinding....*/ 0,
                            /*..uint32_t...............bindingCount....*/ 1,
                            /*..const.VkBuffer*........pBuffers........*/ &(pGeometryBufferSet->vertexBufferInfo.bufferHandle),
                            /*..const.VkDeviceSize*....pOffsets........*/ offsets);

    vkCmdBindIndexBuffer (/*...VkCommandBuffer..........commandBuffer...*/ commandBuffer,
                          /*...VkBuffer.................buffer..........*/ pGeometryBufferSet->indexBufferInfo.bufferHandle,
                          /*...VkDeviceSize.............offset..........*/ 0, //offset is the starting offset in bytes within buffer used in index buffer address calculations
                          /*...VkIndexType..............indexType.......*/ VK_INDEX_TYPE_UINT32);

    for (uint32_t meshIdx = 0; meshIdx < pGeometryBufferSet->numMeshes; meshIdx++)
    {
        const MeshInfo* pMeshInfo = &(pGeometryBufferSet->pMeshes[meshIdx]);

        vkCmdDrawIndexed (/*...VkCommandBuffer.......commandBuffer......*/ commandBuffer,
                          /*...uint32_t..............indexCount.........*/ pMeshInfo->numPrims * SceneVulkanParameters::verticiesPerPrimitive,
                          /*...uint32_t..............instanceCount......*/ 1,
                          /*...uint32_t..............firstIndex.........*/ pMeshInfo->firstPrimIdx * SceneVulkanParameters::verticiesPerPrimitive, // firstIndex is "the base index within the index buffer"....so like idx of the first index in the bufferfor this draw
                          /*...int32_t...............vertexOffset.......*/ 0, // vertexOffset is the value added to the vertex index before indexing into the vertex buffer.
                          /*...uint32_t..............firstInstance......*/ pMeshInfo->materialIdx);
    }

    vkCmdNextSubpass (commandBuffer, VK_SUBPASS_CONTENTS_INLINE);// next subpass


    vkCmdBindPipeline (commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pSubpass1Parameters->pipeline); // bind pipeline

    vkCmdBindDescriptorSets (/*...VkCommandBuffer..........commandBuffer.........*/ commandBuffer,
                             /*...VkPipelineBindPoint......pipelineBindPoint.....*/ VK_PIPELINE_BIND_POINT_GRAPHICS,
                             /*...VkPipelineLayout.........layout................*/ pSubpass1Parameters->pipelineLayout,
                             /*...uint32_t.................firstSet..............*/ 0,
                             /*...uint32_t.................descriptorSetCount....*/ 1,
                             /*...const.VkDescriptorSet*...pDescriptorSets.......*/ &(pSubpass1Parameters->descriptorSet),
                             /*...uint32_t.................dynamicOffsetCount....*/ 0,
                             /*...const.uint32_t*..........pDynamicOffsets.......*/ nullptr);

    vkCmdDraw (commandBuffer, 6, 1, 0, 0); // draw fullscreen quad

    vkCmdEndRenderPass (commandBuffer);

    VkResult result = vkEndCommandBuffer (commandBuffer);
    assert (result == VK_SUCCESS);

    return commandBuffer;
}

VkResult SubmitRenderCommandBuffer (VkCommandBuffer             commandBuffer,
                                    VkQueue                     queue,
                                    PerSwapchainImageResources* pPerSwapchainImageResources)
{

    if (pPerSwapchainImageResources->releaseSwapchainImageSemaphore == VK_NULL_HANDLE)
    {
        pPerSwapchainImageResources->releaseSwapchainImageSemaphore = VkSync::SemaphoreDepot.ObtainSemaphore ();
    }

    VkPipelineStageFlags waitStage  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo         submitInfo =
    {
        /*...VkStructureType................sType...................*/ VK_STRUCTURE_TYPE_SUBMIT_INFO,
        /*...const.void*....................pNext...................*/ 0,
        /*...uint32_t.......................waitSemaphoreCount......*/ 1,
        /*...const.VkSemaphore*.............pWaitSemaphores.........*/ &(pPerSwapchainImageResources->acquireSwapchainImageSemaphore), // Only execute work when this semaphore is in a signaled state
        /*...const.VkPipelineStageFlags*....pWaitDstStageMask.......*/ &waitStage,
        /*...uint32_t.......................commandBufferCount......*/ 1,
        /*...const.VkCommandBuffer*.........pCommandBuffers.........*/ &commandBuffer,
        /*...uint32_t.......................signalSemaphoreCount....*/ 1,
        /*...const.VkSemaphore*.............pSignalSemaphores.......*/ &(pPerSwapchainImageResources->releaseSwapchainImageSemaphore) // Semaphores that will be signaled when the command buffers for this batch have completed execution.
    };

    return vkQueueSubmit (queue, 1, &submitInfo, pPerSwapchainImageResources->queueSubmitFence);
}

VkResult PresentImage(VkSwapchainKHR swapchain,
                      uint32_t       swapchainImageIndex,
                      VkSemaphore    swapchainImageReleaseSemaphore, // Wait for this semaphore to be signaled before presenting the image
                      VkQueue        queue)
{
    VkResult result = VK_NOT_READY;

    VkPresentInfoKHR presentInfo =
    {
        /*...VkStructureType..........sType.................*/ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        /*...const.void*..............pNext.................*/ 0,
        /*...uint32_t.................waitSemaphoreCount....*/ 1,
        /*...const.VkSemaphore*.......pWaitSemaphores.......*/ &swapchainImageReleaseSemaphore,
        /*...uint32_t.................swapchainCount........*/ 1,
        /*...const.VkSwapchainKHR*....pSwapchains...........*/ &swapchain,
        /*...const.uint32_t*..........pImageIndices.........*/ &swapchainImageIndex,
        /*...VkResult*................pResults..............*/ &result
    };

    return vkQueuePresentKHR(queue, &presentInfo);
}

VkDescriptorSet AllocateAndWriteSubpass0DescriptorSet (VkDevice               logicalDevice,
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
        /*...uint32_t........................descriptorSetCount...*/ SceneVulkanParameters::Subpass0::numDescriptorSetsUsedInSubpass,
        /*...const.VkDescriptorSetLayout*....pSetLayouts..........*/ &descriptorSetLayoutHandle
    };

    VkDescriptorSet descriptorSetHandle = VK_NULL_HANDLE;
    VkResult        result = vkAllocateDescriptorSets (logicalDevice,
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

    // Write the descriptor set with info about the resources backing the ubo
    static const uint32_t numDescriptorsToUpdate = 2;
    VkWriteDescriptorSet pDescriptorUpdateWrites[numDescriptorsToUpdate] =
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

    vkUpdateDescriptorSets (/*...VkDevice....................................device.................*/ logicalDevice,
                            /*...uint32_t....................................descriptorWriteCount...*/ numDescriptorsToUpdate,
                            /*...const.VkWriteDescriptorSet*.................pDescriptorWrites......*/ pDescriptorUpdateWrites,
                            /*...uint32_t....................................descriptorCopyCount....*/ 0,
                            /*...const.VkCopyDescriptorSet*..................pDescriptorCopies......*/ nullptr);

    return descriptorSetHandle;


}

// Allocates a descriptor set with 3 descriptors of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
VkDescriptorSet AllocateAndWriteSubpass1DescriptorSet (VkDevice               logicalDevice,
                                                       VkDescriptorPool       descriptorPoolHandle,
                                                       VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                                       VkImageView            diffuseColorImageViewHandle,
                                                       VkImageView            normalVectorImageViewHandle,
                                                       VkImageView            positionVectorImageViewHandle,
                                                       VkBuffer               uniformBufferHandle)
{
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo =
    {
        /*...VkStructureType.................sType................*/ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        /*...const.void*.....................pNext................*/ nullptr,
        /*...VkDescriptorPool................descriptorPool.......*/ descriptorPoolHandle,
        /*...uint32_t........................descriptorSetCount...*/ SceneVulkanParameters::Subpass1::numDescriptorSetsUsedInSubpass,
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

    VkDescriptorImageInfo diffuseColorInputAttachmentDescriptorInfo
    {
        /*...VkSampler........sampler.......*/ VK_NULL_HANDLE,
        /*...VkImageView......imageView.....*/ diffuseColorImageViewHandle,
        /*...VkImageLayout....imageLayout...*/ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkDescriptorImageInfo normalsColorInputAttachmentDescriptorInfo
    {
        /*...VkSampler........sampler.......*/ VK_NULL_HANDLE,
        /*...VkImageView......imageView.....*/ normalVectorImageViewHandle,
        /*...VkImageLayout....imageLayout...*/ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkDescriptorImageInfo positionInputAttachmentDescriptorInfo
    {
        /*...VkSampler........sampler.......*/ VK_NULL_HANDLE,
        /*...VkImageView......imageView.....*/ positionVectorImageViewHandle,
        /*...VkImageLayout....imageLayout...*/ VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    // Write the descriptor set with info about the resources backing the ubo and 4 input attachments
    static const uint32_t numDescriptorsToUpdate = SceneVulkanParameters::Subpass1::numDescriptorSetLayoutBindings; //4
    VkWriteDescriptorSet  pDescriptorUpdateWrites[numDescriptorsToUpdate] =
    {
        { // struct describing the descriptor update for the uniform buffer at binding 0
            /*...VkStructureType..................sType..............*/ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            /*...const.void*......................pNext..............*/ nullptr,
            /*...VkDescriptorSet..................dstSet.............*/ descriptorSetHandle,
            /*...uint32_t.........................dstBinding.........*/ SceneVulkanParameters::Subpass1::uniformBufferBinding, // 0
            /*...uint32_t.........................dstArrayElement....*/ 0,
            /*...uint32_t.........................descriptorCount....*/ 1,
            /*...VkDescriptorType.................descriptorType.....*/ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /*...const.VkDescriptorImageInfo*.....pImageInfo.........*/ nullptr,
            /*...const.VkDescriptorBufferInfo*....pBufferInfo........*/ &uniformBufferDescriptorInfo,
            /*...const.VkBufferView*..............pTexelBufferView...*/ nullptr
        },
        { // struct describing the descriptor update for the diffuse color/albedo gbuffer input attachment
            /*...VkStructureType..................sType..............*/ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            /*...const.void*......................pNext..............*/ nullptr,
            /*...VkDescriptorSet..................dstSet.............*/ descriptorSetHandle,
            /*...uint32_t.........................dstBinding.........*/ SceneVulkanParameters::Subpass1::diffuseColorInputAttachmentBinding, // 1  @note: Reminder: these indeces are different than the renderpass attachment idx.
            /*...uint32_t.........................dstArrayElement....*/ 0,
            /*...uint32_t.........................descriptorCount....*/ 1,
            /*...VkDescriptorType.................descriptorType.....*/ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            /*...const.VkDescriptorImageInfo*.....pImageInfo.........*/ &diffuseColorInputAttachmentDescriptorInfo,
            /*...const.VkDescriptorBufferInfo*....pBufferInfo........*/ nullptr,
            /*...const.VkBufferView*..............pTexelBufferView...*/ nullptr
        },
        { // struct describing the descriptor update for surface normal gbuffer input attachment
            /*...VkStructureType..................sType..............*/ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            /*...const.void*......................pNext..............*/ nullptr,
            /*...VkDescriptorSet..................dstSet.............*/ descriptorSetHandle,
            /*...uint32_t.........................dstBinding.........*/ SceneVulkanParameters::Subpass1::normalVectorInputAttachmentBinding, // 2
            /*...uint32_t.........................dstArrayElement....*/ 0,
            /*...uint32_t.........................descriptorCount....*/ 1,
            /*...VkDescriptorType.................descriptorType.....*/ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            /*...const.VkDescriptorImageInfo*.....pImageInfo.........*/ &normalsColorInputAttachmentDescriptorInfo,
            /*...const.VkDescriptorBufferInfo*....pBufferInfo........*/ nullptr,
            /*...const.VkBufferView*..............pTexelBufferView...*/ nullptr
        },
        { // struct describing the descriptor update for world position gbuffer input attachment
            /*...VkStructureType..................sType..............*/ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            /*...const.void*......................pNext..............*/ nullptr,
            /*...VkDescriptorSet..................dstSet.............*/ descriptorSetHandle,
            /*...uint32_t.........................dstBinding.........*/ SceneVulkanParameters::Subpass1::positionInputAttachmentBinding, // 3
            /*...uint32_t.........................dstArrayElement....*/ 0,
            /*...uint32_t.........................descriptorCount....*/ 1,
            /*...VkDescriptorType.................descriptorType.....*/ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            /*...const.VkDescriptorImageInfo*.....pImageInfo.........*/ &positionInputAttachmentDescriptorInfo,
            /*...const.VkDescriptorBufferInfo*....pBufferInfo........*/ nullptr,
            /*...const.VkBufferView*..............pTexelBufferView...*/ nullptr
        }
    };

    vkUpdateDescriptorSets (/*...VkDevice....................................device.................*/ logicalDevice,
                            /*...uint32_t....................................descriptorWriteCount...*/ numDescriptorsToUpdate,
                            /*...const.VkWriteDescriptorSet*.................pDescriptorWrites......*/ pDescriptorUpdateWrites,
                            /*...uint32_t....................................descriptorCopyCount....*/ 0,
                            /*...const.VkCopyDescriptorSet*..................pDescriptorCopies......*/ nullptr);

    return descriptorSetHandle;

}
#endif // VULKAN_UTILS_CPP