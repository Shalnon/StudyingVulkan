#ifndef STUDYING_VULKAN_CPP
#define STUDYING_VULKAN_CPP

#include "StudyingVulkan.h"
#include "Vulkan_Utils.h"
#include "AppWindowingSystem.h"
#include "resource.h"
#include "Vulkan_Synchronization.h"
#include "Vulkan_Buffer_Tools.h"
#include "Vulkan_Descriptor_Tools.h"
#include "Asset_Tools.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Logging.h"
#include <math.h>  
//#include <thread>

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

    // Creates a VkPhysicalDevice with the specified features and extensions enabled. Also gets graphics queue index.
    CreatePhysicalDeviceAndQueue(/*..VkInstance.................instance...........................*/ instance,
                                 /*..VkSurfaceKHR...............surface............................*/ surface,
                                 /*..uint32_t...................numnRequiredExtensions.............*/ SceneVulkanParameters::numRequiredExtensions,
                                 /*..const char*................ppRequiredDeviceExtensionNames[]...*/ SceneVulkanParameters::requiredDeviceExtensionss,
                                 /*..VkPhysicalDeviceFeatures*..pDeviceFeatures....................*/ &deviceFeatures,
                                 /*..VkPhysicalDevice*..........pPhysicalDeviceOut.................*/ &physicalDevice,
                                 /*..VkQueue*...................pQueueOut..........................*/ &queue,
                                 /*..VkDevice*..................pLogicalDeviceOut..................*/ &logicalDevice,
                                 /*..uint32_t*..................pQueueIndexOut.....................*/ &queueFamilyIndex);

    // Make sure we got queue index from the above function.
    assert (queueFamilyIndex != UINT32_MAX);

    // Now that we have a device, create the semaphore list.
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
    |* Allocates a command buffer for each of the swapchain images
    |* Creates image views for swapchain images
    |* Chooses a supported depth format from the lists above.
    |* Allocates and binds image memory to the depth images.
    |* Creates image views for the depth images.
    |* Creates the images and image views for the surface normals and diffuse color attachments */
    InitializeSwapchain (/*...VkPhysicalDevice.............physicalDevice......................*/ physicalDevice,
                         /*...VkDevice.....................logicalDevice.......................*/ logicalDevice,
                         /*...uint32_t.....................graphicsQueueIndex..................*/ queueFamilyIndex,
                         /*...VkSurfaceKHR.................surface.............................*/ surface,
                         /*...uint32_t.....................numPreferredSurfaceFormats..........*/ numPreferredSurfaceFormats,
                         /*...uint32_t.....................numPreferredDepthFormats............*/ numPreferredDepthFormats,
                         /*...VkFormat*....................pPreferredSurfaceFormats............*/ pPreferredSurfaceFormats,
                         /*...VkFormat*....................pPreferredDepthFormats..............*/ pPreferredDepthFormats,
                         /*...VkExtent2D...................prefferredExtent....................*/ preferredFrameDimensions,
                         /*...VkSwapchainKHR...............oldSwapchain........................*/ VK_NULL_HANDLE,
                         /*...VkSwapchainKHR*..............pSwapchainOut.......................*/ &swapchain,
                         /*...VkExtent2D*..................pSwapchainExtentChosenOut...........*/ &actualFrameDimensions,
                         /*...VkSurfaceFormatKHR*..........pSurfaceFormatOut...................*/ &chosenSurfaceFormat,
                         /*...VkFormat*....................pChosenDepthFormatOut...............*/ &chosenDepthFormat,
                         /*...uint32_t*....................pNumSwapchainImages.................*/ &numSwapChainImages,
                         /*...PerSwapchainImageResources*..ppPerSwapchainImageResourcesInOut...*/ &pPerSwapchainImageResources);

    const uint32_t        numSubpasses                               = SceneVulkanParameters::RenderPassParameters::numSubpasses;
    VkDescriptorSetLayout pSubpassDescriptorSetLayouts[numSubpasses] = { VK_NULL_HANDLE, VK_NULL_HANDLE };

    // Creates the renderpass
    VkRenderPass   renderpassHandle = CreateRenderpass (chosenSurfaceFormat.format, chosenDepthFormat, logicalDevice);

    // Creates descriptor set layouts for both descriptor sets.
    CreateDescriptorSetLayout (logicalDevice,
                               &pSubpassDescriptorSetLayouts[0],
                               &pSubpassDescriptorSetLayouts[1]);

    // Handles for the pipeline layouts created for each pipeline will be filled in below and than used as an arg to vkCmdBindDescriptorSets when recording the command buffers
    VkPipelineLayout pPipelineLayoutHandles[numSubpasses] = { VK_NULL_HANDLE, VK_NULL_HANDLE };

    std::string subpass0FragPath = std::string (pShaderRootDirectory) + std::string (SceneVulkanParameters::Subpass0::pFragShaderPath);
    std::string subpass0VertPath = std::string (pShaderRootDirectory) + std::string (SceneVulkanParameters::Subpass0::pVertShaderPath);
    const char* s0_frag_path = subpass0FragPath.c_str();
    const char* s0_vert_path = subpass0VertPath.c_str ();

    std::string subpass1FragPath = std::string (pShaderRootDirectory) + std::string (SceneVulkanParameters::Subpass1::pFragShaderPath);
    std::string subpass1VertPath = std::string (pShaderRootDirectory) + std::string (SceneVulkanParameters::Subpass1::pVertShaderPath);
    const char* s1_frag_path = subpass1FragPath.c_str ();
    const char* s1_vert_path = subpass1VertPath.c_str ();

    printf ("subpass0 - frag: %s\n", s0_frag_path);
    printf ("subpass0 - vert: %s\n", s0_vert_path);
    printf ("subpass1 - frag: %s\n", s1_frag_path);
    printf ("subpass1 - vert: %s\n", s1_vert_path);

    // Create the pipeline used by subpass 1 to fill the gbuffer images
    VkPipeline subpass0Pipeline = CreateSubpass0Pipeline(logicalDevice,
                                                         renderpassHandle,
                                                         &actualFrameDimensions,
                                                         pSubpassDescriptorSetLayouts[0],
                                                         s0_frag_path,
                                                         s0_vert_path,
                                                         &pPipelineLayoutHandles[0]);

    VkPipeline subpass1Pipeline = CreateSubpass1Pipeline (logicalDevice, 
                                                          renderpassHandle,
                                                          &actualFrameDimensions,
                                                          pSubpassDescriptorSetLayouts[1],
                                                          s1_frag_path,
                                                          s1_vert_path,
                                                          &pPipelineLayoutHandles[1]);

    CreateFrameBuffers(logicalDevice,
                       renderpassHandle,
                       &actualFrameDimensions,
                       numSwapChainImages,
                       pPerSwapchainImageResources);
     
    std::string assetsDirPath = std::string(pAssetDirectory);
    std::string modelPath     = assetsDirPath + std::string ("\\Models\\monkey_with_color.obj");
    

    printf ("model path = %s\n", modelPath.c_str ());
    const aiScene* pScene = aiImportFile (modelPath.c_str (), MY_ASSIMP_PREPROCESSING_FLAGS);

    vulkanAllocatedBufferInfo colorsStorageBufferInfo = CreateMeshColorsStorageBuffer (physicalDevice,
                                                                                       logicalDevice,
                                                                                       queue,
                                                                                       queueFamilyIndex,
                                                                                       pScene);

    const float renderingSurfaceAspectRatio = float (WINDOW_WIDTH) / float (WINDOW_HEIGHT);
    const float fov                         = SceneVulkanParameters::horizontal_fov;
    glm::mat4   projectionMatrix            = glm::perspective (fov,
                                                                renderingSurfaceAspectRatio,
                                                                SceneVulkanParameters::zNear,
                                                                SceneVulkanParameters::zFar);

    // Create a vertex and index buffer
    GeometryBufferSet geometrysBuffers = CreateGeometryBuffersAndAABBs (physicalDevice,
                                                                        logicalDevice,
                                                                        queue,
                                                                        queueFamilyIndex,
                                                                        pScene);

    //@NOTE: Ndc bounds on the z axis are [0,1] for vulkan, whereas for opengl it is [-1,1]
    VkAabbPositionsKHR sceneBounds =
    {
        /*...float....minX...*/ -2.5f,
        /*...float....minY...*/ -2.5f,
        /*...float....minZ...*/  5.0f,
        /*...float....maxX...*/  2.5f,
        /*...float....maxY...*/  2.5f,
        /*...float....maxZ...*/  10.0f
    };


    // Initializing sceneTransformUBO to a transform that will translate and scale the scene such that if fits inside the AABB defined by *pDesiredSceneBounds
    glm::mat4x4 sceneTransform = GetTransform_FitAABBToAABB (/*...VkAabbPositionsKHR...originalAABB...............*/ geometrysBuffers.sceneAABB,
                                                             /*...VkAabbPositionsKHR...desiredBounds..............*/ sceneBounds,
                                                             /*...bool.................maintainSceneAspectRatio...*/ true);

    PrintNamedMatrix ("InitialSceneTransform", &sceneTransform);

    UniformBufferData initialUboBufferData =
    {
        /*...mat4...sceneTransform.........*/ sceneTransform,
        /*...vec4...sceneScale.............*/ glm::vec4 (1.0f,   1.0f,  1.0f, 1.0f),
        /*...mat4...projectionMatrix.......*/ projectionMatrix,
        /*...vec3...ambient_color..........*/ glm::vec4 (0.12f,  0.12f,  0.12f, 1.0f),
        /*...vec3...lightLocation..........*/ glm::vec4 (-2.0f, -2.0f,  -2.0f, 1.0f),
        /*...vec3...lightIntensities.......*/ glm::vec4 (1.0f,   1.0f,  1.0f, 1.0f)
    };


    // Creates UBO and fills it with data. Contains scene transform
    vulkanAllocatedBufferInfo uniformBufferInfo = CreateUniformBuffer (physicalDevice,
                                                                       logicalDevice,
                                                                       queue,
                                                                       queueFamilyIndex,
                                                                       &initialUboBufferData,
                                                                       true);

    VkDescriptorPool descriptorPoolHandle  = CreateDescriptorPool (logicalDevice);

    // Descriptor set containing a storage buffer and uniform buffer descriptor
    // Only need one descriptor set for subpass 1, because the buffers used to back the ssbo and ubo are not per swapchain image, and are not written to either.
    VkDescriptorSet       subpass0DescriptorSet = AllocateAndWriteSubpass0DescriptorSet (logicalDevice,
                                                                                         descriptorPoolHandle,
                                                                                         pSubpassDescriptorSetLayouts[0],
                                                                                         uniformBufferInfo.bufferHandle,
                                                                                         colorsStorageBufferInfo.bufferHandle);

    for (uint32_t swapIdx = 0; swapIdx < numSwapChainImages; swapIdx++)
    {
#ifdef DEBUG
        printf ("Creating descriptor set for swapchainImageResourceSet[%u].\n", swapIdx);
#endif

        PerSwapchainImageResources* pSwapchainImageResourceSet = &pPerSwapchainImageResources[swapIdx];
        // do i need one of these for each swap image?
        VkDescriptorSet             subpass1DescriptorSet = AllocateAndWriteSubpass1DescriptorSet (logicalDevice,
                                                                                                   descriptorPoolHandle,
                                                                                                   pSubpassDescriptorSetLayouts[1],
                                                                                                   pSwapchainImageResourceSet->diffuseImageViewHandle,
                                                                                                   pSwapchainImageResourceSet->normalsImageViewHandle,
                                                                                                   pSwapchainImageResourceSet->positionImageViewHandle,
                                                                                                   pSwapchainImageResourceSet->depthImageViewHandle,
                                                                                                   uniformBufferInfo.bufferHandle);

        pSwapchainImageResourceSet->subpass1DesciptorSetHandle = subpass1DescriptorSet;
    }

    PerSubpassRenderParameters   subpass0Parameters =
    {
        /*...VkDescriptorSet...descriptorSet....*/ subpass0DescriptorSet,
        /*...VkPipeline........pipeline.........*/ subpass0Pipeline,
        /*...VkPipelineLayout..pipelineLayout...*/ pPipelineLayoutHandles[0] // Needed when binding the descriptor set
    };

    PerSubpassRenderParameters   subpass1Parameters =
    {
        /*...VkDescriptorSet...descriptorSet....*/ VK_NULL_HANDLE, // Set inside render loop. Need a swapchain index first to figure out which descriptor set to bind
        /*...VkPipeline........pipeline.........*/ subpass1Pipeline,
        /*...VkPipelineLayout..pipelineLayout...*/ pPipelineLayoutHandles[1] // Needed when binding the descriptor set
    };

    uint32_t numFramesToRender = 5000;
    float    rotationPerFrame  = 0.06;
    float    rotationsPer360   = 360.0f / rotationPerFrame;
    float    numRotations      = 0;


    glm::vec3 currentSceneRotation = glm::vec3 (0.0f, 0.0f, 0.0f);
    float     rotationRate         = 0.04;

    printf("about to start executing renderloop\n");
    for (uint32_t i = 0; i < numFramesToRender; i++)
    {
        currentSceneRotation = glm::vec3(std::fmod(currentSceneRotation.x, 360.0f),
                                         std::fmod(currentSceneRotation.y, 360.0f),
                                         std::fmod(currentSceneRotation.z, 360.0f));

        // We will only be changing some matrices, so we can fill in most of the paramters by setting uboData to initialUboBufferData
        UniformBufferData uboData = initialUboBufferData; //@TODO: get normal rotation matrix
        uboData.sceneTransform = GetSceneTransform(/*...VkAabbPositionsKHR...sceneBounds...............*/ sceneBounds,
                                                   /*...VkAabbPositionsKHR...meshDataAabb..............*/ geometrysBuffers.sceneAABB,
                                                   /*... glm::vec3...........sceneRotation.............*/ currentSceneRotation);

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
                           /*...VkRenderPass.................renderpassHandle...............*/ renderpassHandle,
                           /*...PerSwapchainImageResources*..pSwapchainImageResources.......*/ &pPerSwapchainImageResources,
                           /*...uint32_t*....................pNumSwapchainImages............*/ &numSwapChainImages,
                           /*...VkExtent2D*..................pExtent........................*/ &actualFrameDimensions,
                           /*...GeometryBufferSet*...........pGeometryBufferSet.............*/ &geometrysBuffers,
                           /*...UniformBufferData*...........pUboData.......................*/ &uboData,
                           /*...vulkanAllocatedBufferInfo*...pUniformBufferInfo.............*/ &uniformBufferInfo,
                           /*...uint32_t.....................frameIdx.......................*/ i);


        CheckForAndDispatchWindowEventMessages (); // This makes sure that WndProc gets called when keyboard or mouse buttons are pressed, or any other window events.


        if (KeyStates::rightArrowKeyPressed)
        {
            currentSceneRotation.y -= rotationRate;
        }

        if (KeyStates::leftArrowKeyPressed)
        {
            currentSceneRotation.y += rotationRate;
        }

        if (KeyStates::downArrowKeyPressed)
        {
            currentSceneRotation.x += rotationRate;
        }

        if (KeyStates::upArrowKeyPressed)
        {
            currentSceneRotation.x -= rotationRate;
        }


#ifdef DEBUG
        printf ("\n--Frame %u end--\n",i);
#endif // DEBUG

    }
   
    printf ("Finished executing render loop %u times.\n", numFramesToRender);

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

        if (renderpassHandle != VK_NULL_HANDLE)
        {
            vkDestroyRenderPass (logicalDevice, renderpassHandle, nullptr);
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

#endif // STUDYING_VULKAN_CPP