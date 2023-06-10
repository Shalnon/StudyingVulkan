// StudyingVulkan.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "StudyingVulkan.h"
#include "Vulkan_Utils.h"
#include "AppWindowingSystem.h"
#include "resource.h"
#include "Vulkan_Synchronization.h"
#include "Vulkan_Buffer_Tools.h"
#include "Vulkan_Descriptor_Tools.h"
#include "Asset_Tools.h"

int APIENTRY wWinMain(_In_    HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_     LPWSTR    lpCmdLine,
                     _In_     int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    ShowConsole();

    HWND window_handle = InitWindowInstance(hInstance, nCmdShow, szWindowClass, szTitle, WINDOW_WIDTH, WINDOW_HEIGHT);

    VkInstance    instance = InitializeVulkanAndCreateInstance();
    VkSurfaceKHR  surface  = CreateVkSurface(instance, hInstance, window_handle);

    VkPhysicalDevice         physicalDevice   = VK_NULL_HANDLE; //gpu
    VkQueue                  queue            = VK_NULL_HANDLE;
    VkDevice                 logicalDevice    = VK_NULL_HANDLE;
    VkPhysicalDeviceFeatures deviceFeatures   = {};
    uint32_t                 queueFamilyIndex = UINT32_MAX;

    const char* requiredDeviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    assert(numRequiredExtensions == (sizeof(requiredDeviceExtensions) / sizeof(const char*)));

    CreatePhysicalDeviceAndQueue(/*..VkInstance.................*/ instance,
                                 /*..VkSurfaceKHR...............*/ surface,
                                 /*..uint32_t...................*/ numRequiredExtensions,
                                 /*..char**.....................*/ const_cast<char**>(requiredDeviceExtensions),
                                 /*..VkPhysicalDeviceFeatures*..*/ &deviceFeatures,
                                 /*..VkPhysicalDevice*..........*/ &physicalDevice,
                                 /*..VkQueue*...................*/ &queue,
                                 /*..VkDevice*..................*/ &logicalDevice,
                                 /*..uint32_t*..................*/ &queueFamilyIndex);

    // Make sure we got queue index from the above function.
    assert (queueFamilyIndex != UINT32_MAX);

    //Now that we have a device, create the semaphore list.
    VkSync::InitSemaphoreList(logicalDevice);

    VkExtent2D                   preferredFrameDimensions    = {WINDOW_WIDTH, WINDOW_HEIGHT};
    VkExtent2D                   actualFrameDimensions       = {};
    VkSurfaceFormatKHR           chosenSurfaceFormat         = {};
    VkSwapchainKHR               swapchain                   = VK_NULL_HANDLE;
    uint32_t                     numSwapChainImages          = 0;
    PerSwapchainImageResources*  pPerSwapchainImageResources = 0;

    // Defines a list of formats that meet our needs. One color, and one depth fromat will be chosen inside InitializeSwapchain.
    const uint32_t    numPreferredSurfaceFormats   = SceneVulkanParameters::numPreferredDepthFormats;
    const VkFormat*   pPreferredSurfaceFormats     = SceneVulkanParameters::preferredSwapchainImagesFormats;
    const uint32_t    numPreferredDepthFormats     = SceneVulkanParameters::numPreferredDepthFormats;
    const VkFormat*   pPreferredDepthFormats       = SceneVulkanParameters::preferredDeptAttachmentFormats;

    VkFormat chosenDepthFormat = VK_FORMAT_UNDEFINED;

    /* InitializeSwapchain () does the following :
    |* Chooses a supported surface format from the lists above.
    |* Establishes how many swapchain images we will be using
    |* Sets up swapchain images as color attachments and creates the swapchain
    |* Creates a fence for each swapchain image that can be used to find when that image is no longer being used by GPU
    |* Creates Command Pool
    |* Allocates a command buffer for each of the swapchain images  -- @todo: pull this out? not sure if it fits this otherwise image focused function
    |* Creates image views for swapchain images
    |* Chooses a supported depth format from the lists above.
    |* Allocates and binds image memory to the depth images.
    |* Creates image views for the depth images.
    |* Creates the images and image views for the surface normals and diffuse color attachments */
    InitializeSwapchain (/*...VkPhysicalDevice.............physicalDevice.................*/ physicalDevice,
                         /*...VkDevice.....................logicalDevice..................*/ logicalDevice,
                         /*...uint32_t.....................graphicsQueueIndex.............*/ queueFamilyIndex,
                         /*...VkSurfaceKHR.................surface........................*/ surface,
                         /*...uint32_t.....................numPreferredSurfaceFormats.....*/ numPreferredSurfaceFormats,
                         /*...uint32_t.....................numPreferredDepthFormats.......*/ numPreferredDepthFormats,
                         /*...VkFormat*....................pPreferredSurfaceFormats.......*/ pPreferredSurfaceFormats,
                         /*...VkFormat*....................pPreferredDepthFormats.........*/ pPreferredDepthFormats,
                         /*...VkExtent2D...................prefferredExtent...............*/ preferredFrameDimensions,
                         /*...VkSwapchainKHR...............oldSwapchain...................*/ VK_NULL_HANDLE,
                         /*...VkSwapchainKHR*..............pSwapchainOut..................*/ &swapchain,
                         /*...VkExtent2D*..................pSwapchainExtentChosenOut......*/ &actualFrameDimensions,
                         /*...VkSurfaceFormatKHR*..........pSurfaceFormatOut..............*/ &chosenSurfaceFormat,
                         /*...VkFormat*....................pChosenDepthFormatOut..........*/ &chosenDepthFormat,
                         /*...uint32_t*....................pNumSwapchainImages............*/ &numSwapChainImages,
                         /*...PerSwapchainImageResources**.ppPerSwapchainImageResources...*/ &pPerSwapchainImageResources);

    VkRenderPass          renderpass   = CreateRenderpass (chosenSurfaceFormat.format, chosenDepthFormat, logicalDevice);
    const uint32_t        numSubpasses = SceneVulkanParameters::RenderPassParameters::numSubpasses;

    VkDescriptorSetLayout pSubpassDescriptorSetLayouts[numSubpasses] = { VK_NULL_HANDLE, VK_NULL_HANDLE };

    CreateDescriptorSetLayout (logicalDevice,
                               &pSubpassDescriptorSetLayouts[0],
                               &pSubpassDescriptorSetLayouts[1]);

    VkPipelineLayout      pPipelineLayoutHandles[numSubpasses] = { VK_NULL_HANDLE, VK_NULL_HANDLE };
    VkPipeline            subpass0Pipeline = CreateSubpass0Pipeline(logicalDevice,   //@todo: CReate separate function for seaprate pipelines
                                                                                          renderpass,
                                                                                          &actualFrameDimensions,
                                                                                          pSubpassDescriptorSetLayouts[0],
                                                                                          SceneVulkanParameters::Subpass0::pFragShaderPath,
                                                                                          SceneVulkanParameters::Subpass0::pVertShaderPath,
                                                                                          &pPipelineLayoutHandles[0]);

    VkPipeline      subpass1Pipeline = CreateSubpass1Pipeline (logicalDevice,   //@todo: CReate separate function for seaprate pipelines
                                                               renderpass,
                                                               &actualFrameDimensions,
                                                               pSubpassDescriptorSetLayouts[1],
                                                               SceneVulkanParameters::Subpass1::pFragShaderPath,
                                                               SceneVulkanParameters::Subpass1::pVertShaderPath,
                                                               &pPipelineLayoutHandles[1]);

    CreateFrameBuffers(logicalDevice,
                       renderpass,
                       &actualFrameDimensions,
                       numSwapChainImages,
                       pPerSwapchainImageResources);
     
    std::string currentPath   = std::filesystem::current_path ().string ();
    std::string assetsDirPath = currentPath   + std::string ("\\..\\..\\..\\Assets");
    std::string modelPath     = assetsDirPath + std::string ("\\Models\\monkey_with_color.obj");

    printf ("model path = %s\n", modelPath.c_str ());

    const aiScene* pScene = aiImportFile (modelPath.c_str (), MY_ASSIMP_PREPROCESSING_FLAGS);//aiProcessPreset_TargetRealtime_MaxQuality);

    vulkanAllocatedBufferInfo colorsStorageBufferInfo = CreateMeshColorsStorageBuffer (physicalDevice,
                                                                                       logicalDevice,
                                                                                       queue,
                                                                                       queueFamilyIndex,
                                                                                       pScene);

    // Create a vertex and index buffer
    GeometryBufferSet         geometrysBuffers     = CreateGeometryBuffersAndAABBs (physicalDevice,
                                                                                    logicalDevice,
                                                                                    queue,
                                                                                    queueFamilyIndex,
                                                                                    pScene);

    //@NOTE: Ndc bounds on the z axis are [0,1] for vulkan, whereas for opengl it is [-1,1]
    VkAabbPositionsKHR sceneBounds =
    {
        /*...float....minX...*/ -1.0f,
        /*...float....minY...*/ -1.0f,
        /*...float....minZ...*/  0.0f,
        /*...float....maxX...*/  1.0f,
        /*...float....maxY...*/  1.0f,
        /*...float....maxZ...*/  1.0f
    };
    // Creates UBO and fills it with data. Contains scene transform
    vulkanAllocatedBufferInfo uniformBufferInfo    = CreateUniformBuffer (physicalDevice,
                                                                          logicalDevice,
                                                                          queue,
                                                                          queueFamilyIndex,
                                                                          &geometrysBuffers,
                                                                          &sceneBounds,
                                                                          true);

    VkDescriptorPool      descriptorPoolHandle  = CreateDescriptorPool (logicalDevice);

    // Descriptor set containing a storage buffer and uniform buffer descriptor
    // Only need one descriptor set for subpass 1, because the buffers used to back the ssbo and ubo are not per swapchain image, and are not written to either.
    VkDescriptorSet       subpass0DescriptorSet = AllocateAndWriteSubpass0DescriptorSet (logicalDevice,
                                                                                         descriptorPoolHandle,
                                                                                         pSubpassDescriptorSetLayouts[0],
                                                                                         uniformBufferInfo.bufferHandle,
                                                                                         colorsStorageBufferInfo.bufferHandle);

    for (uint32_t swapIdx = 0; swapIdx < numSwapChainImages; swapIdx++)
    {
        printf ("Creating descriptor set for swapchainImageResourceSet[%u].\n", swapIdx);
        PerSwapchainImageResources* pSwapchainImageResourceSet = &pPerSwapchainImageResources[swapIdx];
        // do i need one of these for each swap image?
        VkDescriptorSet             subpass1DescriptorSet = AllocateAndWriteSubpass1DescriptorSet (logicalDevice,
                                                                                                   descriptorPoolHandle,
                                                                                                   pSubpassDescriptorSetLayouts[1],
                                                                                                   pSwapchainImageResourceSet->diffuseImageViewHandle,
                                                                                                   pSwapchainImageResourceSet->normalsImageViewHandle,
                                                                                                   pSwapchainImageResourceSet->depthImageViewHandle,
                                                                                                   uniformBufferInfo.bufferHandle);


        pSwapchainImageResourceSet->subpass1DesciptorSetHandle = subpass1DescriptorSet;
    }

    PerSubpassRenderParameters   subpass0Parameters =
    {
        /*...VkDescriptorSet...descriptorSet....*/ subpass0DescriptorSet,
        /*...VkPipeline........pipeline.........*/ subpass0Pipeline,
        /*...VkPipelineLayout..pipelineLayout...*/ pPipelineLayoutHandles[0]
    };

    PerSubpassRenderParameters   subpass1Parameters =
    {
        /*...VkDescriptorSet...descriptorSet....*/ VK_NULL_HANDLE, // Set inside render loop. Need a swapchain index first to figure out which descriptor set to bind
        /*...VkPipeline........pipeline.........*/ subpass1Pipeline,
        /*...VkPipelineLayout..pipelineLayout...*/ pPipelineLayoutHandles[1]
    };


    uint32_t numFramesToRender = 5;

    printf("about to start executing renderloop\n");
    for (uint32_t i = 0; i < numFramesToRender; i++)
    {
        printf ("\n--Frame %u begin--\n", i);
        ExecuteRenderLoop (/*...VkDevice.....................logicalDevice..................*/ logicalDevice,
                           /*...VkPhysicalDevice.............physicalDevice,................*/ physicalDevice,
                           /*...VkSwapchainKHR...............swapchain......................*/ swapchain,
                           /*...VkQueue......................queue..........................*/ queue,
                           /*...PerSubpassRenderParameters*..pSubpass0Parameters............*/ &subpass0Parameters,
                           /*...PerSubpassRenderParameters*..pSubpass1Parameters............*/ &subpass1Parameters,
                           /*...uint32_t.....................gfxQueueIdx....................*/ queueFamilyIndex,
                           /*...uint32_t.....................numPreferredSwapchainFormats...*/ numPreferredSurfaceFormats,
                           /*...uint32_t.....................numPreferredDepthFormats.......*/ numPreferredDepthFormats,
                           /*...VkFormat*....................pPreferredSwapchainFormats.....*/ pPreferredSurfaceFormats,
                           /*...VkFormat*....................pPreferredDepthFormats.........*/ pPreferredDepthFormats,
                           /*...VkSurfaceKHR.................surface........................*/ surface,
                           /*...VkRenderPass.................renderpass.....................*/ renderpass,
                           /*...PerSwapchainImageResources**.ppSwapchainImageResources......*/ &pPerSwapchainImageResources, //@TODO: dont think this actually needs to be a double ptr
                           /*...uint32_t*....................pNumSwapchainImages............*/ &numSwapChainImages,
                           /*...VkExtent2D*..................pExtent........................*/ &actualFrameDimensions,
                           /*...GeometryBufferSet*...........pGeometryBufferSet.............*/ &geometrysBuffers,
                           /*...uint32_t.....................frameIdx)......................*/ i);

        printf ("\n--Frame %u end--\n",i);
    }
   
    printf ("finished executing render loop %u times.\n", numFramesToRender);

    // Destroying vk objects below. Also wraping the destroy calls in an extra {} scope just so it can be collapsed easily in an IDE
    {
        // Destroy vk resources tracked in PerSwapchainImageResources structures
        for (uint32_t imgIdx = 0; imgIdx < numSwapChainImages; imgIdx++)
        {
            PerSwapchainImageResources* pSwapImageResources = &(pPerSwapchainImageResources[imgIdx]);

            if (pSwapImageResources->queueSubmitFence != VK_NULL_HANDLE)
            {
                vkDestroyFence (logicalDevice, pSwapImageResources->queueSubmitFence, nullptr);
            }

            if (pSwapImageResources->commandBuffer != VK_NULL_HANDLE)
            {
                vkFreeCommandBuffers (logicalDevice, pSwapImageResources->commandPool, 1, &(pSwapImageResources->commandBuffer));
            }

            if (pSwapImageResources->commandPool != VK_NULL_HANDLE)
            {
                vkFreeCommandBuffers (logicalDevice, pSwapImageResources->commandPool, 1, &pSwapImageResources->commandBuffer);
            }

            if (pSwapImageResources->presentImageViewHandle != VK_NULL_HANDLE)
            {
                vkDestroyImageView (logicalDevice, pSwapImageResources->presentImageViewHandle, nullptr);
            }

            if (pSwapImageResources->framebufferHandle != VK_NULL_HANDLE)
            {
                vkDestroyFramebuffer (logicalDevice, pSwapImageResources->framebufferHandle, nullptr);
            }
        }

        /*
        if (pipelineLayout != VK_NULL_HANDLE)
        {
            vkDestroyPipelineLayout(...)
        {
        */

        if (subpass0Pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline (logicalDevice, subpass0Pipeline, nullptr);
        }

        if (subpass1Pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline (logicalDevice, subpass1Pipeline, nullptr);
        }

        if (swapchain != VK_NULL_HANDLE)
        {
            vkDestroySwapchainKHR (logicalDevice, swapchain, nullptr);
        }

        if (renderpass != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass (logicalDevice, renderpass, nullptr);
        }

        if (logicalDevice != VK_NULL_HANDLE)
        {
            vkDestroyDevice (logicalDevice, nullptr);
        }

        if (instance != VK_NULL_HANDLE)
        {
            vkDestroyInstance (instance, nullptr);
        }

    }
    return 0;
}

/// if we didnt have this, and just used wWinMain as the entry point, we would need to use the
///   visual studio windows application subsystem, and we would need to create our own console.
///    a console we create wont have the vulkan layer output present, and it wont get captured easily by vulkan configurator.
int main ()
 {
    LPWSTR  pCommandLine          = GetCommandLineW ();
    int32_t numCmdLineArgs        = 0;
    LPWSTR* pCommangLineArgvArray = CommandLineToArgvW (pCommandLine, &numCmdLineArgs);

    return wWinMain (GetModuleHandle (NULL), NULL, GetCommandLineW (), SW_SHOWNORMAL);
 }
