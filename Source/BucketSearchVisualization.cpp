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

#include "BucketSearchVisualization.h"
#include "Vulkan_Utils.h"
#include "AppWindowingSystem.h"
#include "resource.h"
#include "Vulkan_Synchronization.h"
#include "Vulkan_compute.h"
#include "config.h"
#include <string>

int APIENTRY wWinMain(_In_    HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_     LPWSTR    lpCmdLine,
                     _In_     int       nCmdShow)
{
    ShowConsole();

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    HWND window_handle = InitWindowInstance(hInstance, nCmdShow, szWindowClass, szTitle, WINDOW_WIDTH, WINDOW_HEIGHT);

    std::string pCurrentPath = std::filesystem::current_path().string();
   
    printf("current path = %s\n", pCurrentPath.c_str());

    std::string vertexShaderPath   = std::string (SHADER_ROOT_DIR) + std::string(VERT_PATH);
    std::string fragmentShaderPath = std::string (SHADER_ROOT_DIR) + std::string(FRAG_PATH);
    std::string computeShaderPath  = std::string (SHADER_ROOT_DIR) + std::string (COMP_PATH);

    printf("Vertex shader path: %s\n",   vertexShaderPath.c_str());
    printf("Fragment shader path: %s\n", fragmentShaderPath.c_str());


    VkInstance    instance = InitializeVulkanAndCreateInstance();
    VkSurfaceKHR  surface  = CreateVkSurface(instance, hInstance, window_handle);

    VkPhysicalDevice         physicalDevice = VK_NULL_HANDLE; //gpu
    VkQueue                  queue          = VK_NULL_HANDLE;
    VkDevice                 logicalDevice  = VK_NULL_HANDLE;
    VkPhysicalDeviceFeatures deviceFeatures = {};
    uint32_t                 queueIndex     = UINT32_MAX;

    const char* requiredDeviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    assert(numRequiredExtensions == sizeof(requiredDeviceExtensions) / sizeof(const char*));

    CreatePhysicalDeviceAndQueue(/*..VkInstance.................*/ instance,
                                 /*..VkSurfaceKHR...............*/ surface,
                                 /*..uint32_t...................*/ numRequiredExtensions,
                                 /*..char**.....................*/ const_cast<char**>(requiredDeviceExtensions),
                                 /*..VkPhysicalDeviceFeatures*..*/ &deviceFeatures,
                                 /*..VkPhysicalDevice*..........*/ &physicalDevice,
                                 /*..VkQueue*...................*/ &queue,
                                 /*..VkDevice*..................*/ &logicalDevice,
                                 /*..uint32_t*..................*/ &queueIndex);


    std::vector<uint32_t> histogram = {};

    // Create and dispatch computer shader once
    RunComputeExample (computeShaderPath.c_str(), 
                       physicalDevice,
                       logicalDevice,
                       queueIndex,
                       queue,
                       histogram);

    uint32_t                  numVertices  = histogram.size () * 6;
    uint32_t                  bufferSize   = numVertices * sizeof (glm::vec3);
    vulkanAllocatedBufferInfo vertexBuffer = CreateAndAllocateDeviceLocalBuffer (physicalDevice,
                                                                                 logicalDevice,
                                                                                 bufferSize,
                                                                                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                                                 queueIndex);

    std::vector<Bar> barGraphMeshData = CreateBarGraphMesh (histogram.data (), histogram.size ());

    vulkanAllocatedBufferInfo vertexStagingBuffer = CreateAndAllocaStagingBuffer (physicalDevice, logicalDevice, bufferSize, queueIndex);

    glm::vec3* pVertexBufferMem = static_cast<glm::vec3*>(MapBufferMemory (vertexStagingBuffer, logicalDevice));
    uint32_t   numVerticesWritten = 0;

    for (Bar& bar : barGraphMeshData)
    {
        for (uint32_t i = 0; i < 6; i++)
        {
            pVertexBufferMem[numVerticesWritten++] = bar.verts[i];

        }
    }

    vkUnmapMemory (logicalDevice, vertexStagingBuffer.memoryHandle);

    ExecuteBuffer2BufferCopy (physicalDevice, logicalDevice, queue, queueIndex, bufferSize, vertexStagingBuffer, vertexBuffer);

    vkQueueWaitIdle (queue);
    // Make sure we got queue index from the above function.
    assert (queueIndex != UINT32_MAX);

    //Now that we have a device, create the semaphore list.
    VkSync::InitSemaphoreList(logicalDevice);

    VkExtent2D                   preferredFrameDimensions    = {WINDOW_WIDTH, WINDOW_HEIGHT};
    VkExtent2D                   actualFrameDimensions       = {};
    VkSurfaceFormatKHR           surfaceFormat               = {};
    VkSwapchainKHR               swapchain                   = VK_NULL_HANDLE;
    uint32_t                     numSwapChainImages          = 0;
    PerSwapchainImageResources*  pPerSwapchainImageResources = 0;

    static const uint32_t    numPreferredSurfaceFormats                           = NUM_PREFERRED_SURFACE_FORMATS;
    VkFormat                 pPreferredSurfaceFormats[numPreferredSurfaceFormats] = PREFERRED_SURRFACE_FORMATS;

    InitializeSwapchain (/*.VkPhysicalDevice.............physicalDevice...............*/ physicalDevice,
                         /*.VkDevice.....................logicalDevice................*/ logicalDevice,
                         /*.uint32_t.....................graphicsQueueIndex...........*/ queueIndex,
                         /*.VkSurfaceKHR.................surface......................*/ surface,
                         /*.uint32_t.....................numPreferredSurfaceFormats...*/ numPreferredSurfaceFormats,
                         /*.VkFormat*....................pPreferredSurfaceFormats.....*/ pPreferredSurfaceFormats,
                         /*.VkExtent2D...................prefferredExtent.............*/ preferredFrameDimensions,
                         /*.VkSwapchainKHR...............oldSwapchain.................*/ VK_NULL_HANDLE,
                         /*.VkSwapchainKHR*..............pSwapchainOut................*/ &swapchain,
                         /*.VkExtent2D*..................pSwapchainExtentChosenOut....*/ &actualFrameDimensions,
                         /*.VkSurfaceFormatKHR*..........pSurfaceFormatOut............*/ &surfaceFormat,
                         /*.uint32_t*....................pNumSwapchainImages..........*/ &numSwapChainImages,
                         /*.PerSwapchainImageResources**.ppPerSwapchainImageResources.*/ &pPerSwapchainImageResources);

    VkRenderPass renderpass = CreateRenderpass(surfaceFormat.format, logicalDevice);
    VkPipeline   pipeline   = CreatePipeline(logicalDevice, 
                                             renderpass,
                                             &actualFrameDimensions,
                                             fragmentShaderPath.c_str(),
                                             vertexShaderPath.c_str());

    CreateFrameBuffers(logicalDevice,
                       renderpass,
                       &actualFrameDimensions,
                       numSwapChainImages,
                       pPerSwapchainImageResources);

    uint32_t numFramesToRender = 5;

    
    printf("executed renderloop\n");
    for (uint32_t i = 0; i < numFramesToRender; i++)
    {
        ExecuteRenderLoop ( /*.VkDevice.....................logicalDevice................*/ logicalDevice,
                            /*.VkPhysicalDevice.............physicalDevice,..............*/ physicalDevice,
                            /*.VkSwapchainKHR...............swapchain....................*/ swapchain,
                            /*.VkQueue......................queue........................*/ queue,
                            /*.uint32_t.....................gfxQueueIdx..................*/ queueIndex,
                            /*.uint32_t.....................numPreferredSwapchainFormats.*/ numPreferredSurfaceFormats,
                            /*.VkFormat*....................pPreferredSwapchainFormats...*/ pPreferredSurfaceFormats,
                            /*.VkSurfaceKHR.................surface......................*/ surface,
                            /*.VkRenderPass.................renderpass...................*/ renderpass,
                            /*.VkPipeline...................pipeline.....................*/ pipeline,
                            /*.PerSwapchainImageResources**.ppPerSwapchainImageResources.*/ &pPerSwapchainImageResources,
                            /*.uint32_t*....................pNumSwapchainImages..........*/ &numSwapChainImages,
                            /*.VkExtent2D*..................pFramebufferExtent...........*/ &actualFrameDimensions,
                            /*.uint32_t.....................frameIdx.....................*/ i,
                            /*.vulkanAllocatedBufferInfo....vertexBuffer.................*/ vertexBuffer,
                            /*.uint32_t.....................numTriangles.................*/ histogram.size() * 2);
    }
   
    // Destroying vk objects below. Using a random scope here just so it can be collapsed easily in an IDE
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

            if (pSwapImageResources->imageView != VK_NULL_HANDLE)
            {
                vkDestroyImageView (logicalDevice, pSwapImageResources->imageView, nullptr);
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

        if (pipeline != VK_NULL_HANDLE)
        {
            vkDestroyPipeline (logicalDevice, pipeline, nullptr);
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
int main (int argc, char* argv[])
 {
    if (argc > 0)
    {
        for (uint32_t i = 1; i < argc; i++)
        {
            size_t argAsNum = std::stoi (std::string(argv[i]));
            if ((argAsNum > ComputeParameters::minBinSize) && (argAsNum <= ComputeParameters::maxBinSize))
            {
                ComputeParameters::binSize = argAsNum;
                break;
            }
        }

        printf ("Using a bin size of %u\n", ComputeParameters::binSize);
    }
    return wWinMain (GetModuleHandle (NULL), NULL, GetCommandLineW (), SW_SHOWNORMAL);
 }
