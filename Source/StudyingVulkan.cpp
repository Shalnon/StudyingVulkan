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
    std::string modelPath     = assetsDirPath + std::string ("\\Models\\SteamCrane\\SteamWreckingCrane.obj");
    

    printf ("model path = %s\n", modelPath.c_str ());
    const aiScene* pScene = aiImportFile (modelPath.c_str (), MY_ASSIMP_PREPROCESSING_FLAGS);

    vulkanAllocatedBufferInfo colorsStorageBufferInfo = CreateMeshColorsStorageBuffer (/*.VkPhysicalDevice..physicalDevice.......*/ physicalDevice,
                                                                                       /*.VkDevice..........logicalDevice........*/ logicalDevice,
                                                                                       /*.VkQueue...........queue................*/ queue,
                                                                                       /*.uint32_t..........graphicsQueueIndex...*/ queueFamilyIndex,
                                                                                       /*.const.aiScene*....pScene...............*/ pScene);

    const float renderingSurfaceAspectRatio = float (WINDOW_HEIGHT) / float (WINDOW_WIDTH);
    glm::mat4   projectionMatrix            = GetProjection (renderingSurfaceAspectRatio,
                                                             SceneVulkanParameters::horizontal_fov);

    // Create a vertex and index buffer
    GeometryBufferSet geometrysBuffers = CreateGeometryBuffersAndAABBs (/*.VkPhysicalDevice..physicalDevice.......*/ physicalDevice,
                                                                        /*.VkDevice..........logicalDevice........*/ logicalDevice,
                                                                        /*.VkQueue...........queue................*/ queue,
                                                                        /*.uint32_t..........graphicsQueueIndex...*/ queueFamilyIndex,
                                                                        /*.const.aiScene*....pScene...............*/ pScene);

    VkAabbPositionsKHR sceneBounds =
    {
        /*...float....minX...*/ -6.5f,
        /*...float....minY...*/ -6.5f,
        /*...float....minZ...*/  1.0f,
        /*...float....maxX...*/  6.5f,
        /*...float....maxY...*/  6.5f,
        /*...float....maxZ...*/  14.0f
    };

    // Initializing sceneTransformUBO to a transform that will translate and scale the scene such that if fits inside the AABB defined by *pDesiredSceneBounds
    glm::mat4x4 sceneTransform = GetTransform_FitAABBToAABB (/*...VkAabbPositionsKHR...originalAABB...............*/ geometrysBuffers.sceneAABB,
                                                             /*...VkAabbPositionsKHR...desiredBounds..............*/ sceneBounds,
                                                             /*...bool.................maintainSceneAspectRatio...*/ true);

#ifdef DEBUG
    PrintNamedMatrix ("InitialSceneTransform", &sceneTransform);
#endif

    UniformBufferData initialUboBufferData =
    {
        /*...mat4...sceneTransform.........*/ sceneTransform,
        /*...vec4...sceneScale.............*/ glm::vec4 (1.0f,   1.0f,  1.0f, 1.0f),
        /*...mat4...projectionMatrix.......*/ projectionMatrix,
        /*...mat4...normalRotation.........*/ glm::identity<glm::mat4> (),
        /*...vec3...ambient_color..........*/ glm::vec4 (0.12f,  0.12f,  0.12f, 1.0f),
        /*...vec3...lightLocation..........*/ glm::vec4 (-2.0f, 4.0f,  -2.0f, 1.0f),
        /*...vec3...lightIntensities.......*/ glm::vec4 (1.0f,   1.0f,  1.0f, 1.0f)
    };

    // Creates UBO and fills it with data. Contains scene transform
    vulkanAllocatedBufferInfo uniformBufferInfo = CreateUniformBuffer (physicalDevice,
                                                                       logicalDevice,
                                                                       queue,
                                                                       queueFamilyIndex,
                                                                       &initialUboBufferData,
                                                                       true);

    // Allocates a descriptor pool with enough descriptors to back all the desctorSets we need.
    VkDescriptorPool descriptorPoolHandle  = CreateDescriptorPool (logicalDevice, numSwapChainImages);

    // Descriptor set containing a storage buffer and uniform buffer descriptor:
    //   || Only need one descriptor set for the first subpass is needed because the buffers used to back the ssbo and ubo
    //   || are not just read from, and the resources it references do not contain anything that is needed on a per-swapchain-image
    VkDescriptorSet       subpass0DescriptorSet = AllocateAndWriteSubpass0DescriptorSet (logicalDevice,
                                                                                         descriptorPoolHandle,
                                                                                         pSubpassDescriptorSetLayouts[0],
                                                                                         uniformBufferInfo.bufferHandle,
                                                                                         colorsStorageBufferInfo.bufferHandle);

    // We will create multiple instances of the resources that are algorithmically tied to a specific swapchain image.
    //   || Per-swapchain-image descriptor-sets will be created that can be bound when recording commands that will render to said image,
    //   || Pointing the shader to the correct swapchain-image-specific buffers/images.
    for (uint32_t swapIdx = 0; swapIdx < numSwapChainImages; swapIdx++)
    {
#ifdef DEBUG
        printf ("Creating descriptor set for swapchainImageResourceSet[%u].\n", swapIdx);
#endif

        PerSwapchainImageResources* pSwapchainImageResourceSet = &pPerSwapchainImageResources[swapIdx];
        VkDescriptorSet             subpass1DescriptorSet      = AllocateAndWriteSubpass1DescriptorSet (logicalDevice,
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

    bool      exitKeyPressed       = false;
    glm::vec3 currentSceneRotation = glm::vec3 (0.0f, 0.0f, 0.0f);  // rotation around each respective axis (xyz)
    float     rotationRate         = 0.04;                          // Amount of rotation applied per frame when a rotation key is pressed

#ifdef DEBUG
    printf("about to start executing renderloop\n");
#endif

    while (exitKeyPressed == false)
    {
        currentSceneRotation = glm::vec3 (std::fmod (currentSceneRotation.x, 360.0f),
                                          std::fmod (currentSceneRotation.y, 360.0f),
                                          std::fmod (currentSceneRotation.z, 360.0f));

        // We will only be changing some matrices, so we can fill in most of the paramters by setting uboData to initialUboBufferData
        UniformBufferData uboData = initialUboBufferData;

        uboData.sceneTransform = GetSceneTransform (/*...VkAabbPositionsKHR...sceneBounds...............*/ sceneBounds,
                                                    /*...VkAabbPositionsKHR...meshDataAabb..............*/ geometrysBuffers.sceneAABB,
                                                    /*...glm::vec3............sceneRotation.............*/ currentSceneRotation,
                                                    /*...glm::mat4*...........pNormalRotationOut........*/ &uboData.normalRotation);

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
                           /*...vulkanAllocatedBufferInfo*...pUniformBufferInfo.............*/ &uniformBufferInfo);


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

        if (KeyStates::escapeKeyPressed)
        {
            exitKeyPressed = true;
        }

#ifdef DEBUG
        printf ("\n--Frame %u end--\n",i);
#endif // DEBUG

    }

    //@TODO: Enable VK_KHR_present_wait and call vkWaitForPresentKHR so that any pending rendering work finishes before we start destroying things.
    
    {// Destroying vk objects below.

        // Destroy vk resources tracked in PerSwapchainImageResources structures first
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

            // If nothing has gone wrong already, than it can probably be safely assumed that if the image handle is not null,
            //   than the memory and view are also not null.
            if (pSwapImageResources->diffuseImageHandle != VK_NULL_HANDLE)
            {
                vkDestroyImage (logicalDevice, pSwapImageResources->diffuseImageHandle, nullptr);
                vkFreeMemory (logicalDevice, pSwapImageResources->diffuseImageMemoryHandle, nullptr);
                vkDestroyImageView (logicalDevice, pSwapImageResources->diffuseImageViewHandle, nullptr);
            }

            if (pSwapImageResources->normalsImageHandle != VK_NULL_HANDLE)
            {
                vkDestroyImage (logicalDevice, pSwapImageResources->normalsImageHandle, nullptr);
                vkFreeMemory (logicalDevice, pSwapImageResources->normalsImageMemoryHandle, nullptr);
                vkDestroyImageView (logicalDevice, pSwapImageResources->normalsImageViewHandle, nullptr);
            }

            if (pSwapImageResources->positionImageHandle != VK_NULL_HANDLE)
            {
                vkDestroyImage (logicalDevice, pSwapImageResources->positionImageHandle, nullptr);
                vkFreeMemory (logicalDevice, pSwapImageResources->positionImageMemoryHandle, nullptr);
                vkDestroyImageView (logicalDevice, pSwapImageResources->positionImageViewHandle, nullptr);
            }

            if (pSwapImageResources->framebufferHandle)
            {
                vkDestroyFramebuffer (logicalDevice, pSwapImageResources->framebufferHandle, nullptr);
            }

            if (pSwapImageResources->bufferUpdatesStagingBuffer != VK_NULL_HANDLE)
            {
                vkDestroyBuffer (logicalDevice, pSwapImageResources->bufferUpdatesStagingBuffer, nullptr);
                vkFreeMemory (logicalDevice, pSwapImageResources->bufferUpdatesStagingMemory, nullptr);
                pSwapImageResources->updatesStagingBufferSize = 0;
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