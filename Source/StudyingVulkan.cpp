// StudyingVulkan.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "StudyingVulkan.h"
#include "Vulkan_Utils.h"
#include "AppWindowingSystem.h"
#include "resource.h"
#include "Vulkan_Synchronization.h"
#include "Vulkan_Buffer_Tools.h"
#include "Asset_Tools.h"

int APIENTRY wWinMain(_In_    HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_     LPWSTR    lpCmdLine,
                     _In_     int       nCmdShow)
{
    ShowConsole();

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    HWND window_handle = InitWindowInstance(hInstance, nCmdShow, szWindowClass, szTitle, WINDOW_WIDTH, WINDOW_HEIGHT);

    std::string vertexShaderPath   = std::string(VERT_PATH); //@TODO: change these to use a relative path from the working directory
    std::string fragmentShaderPath = std::string(FRAG_PATH);

    printf("Vertex shader path: %s\n",   vertexShaderPath.c_str());
    printf("Fragment shader path: %s\n", fragmentShaderPath.c_str());


    VkInstance    instance = InitializeVulkanAndCreateInstance();
    VkSurfaceKHR  surface  = CreateVkSurface(instance, hInstance, window_handle);

    VkPhysicalDevice         physicalDevice   = VK_NULL_HANDLE; //gpu
    VkQueue                  queue            = VK_NULL_HANDLE;
    VkDevice                 logicalDevice    = VK_NULL_HANDLE;
    VkPhysicalDeviceFeatures deviceFeatures   = {};
    uint32_t                 queueFamilyIndex = UINT32_MAX;

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
                                 /*..uint32_t*..................*/ &queueFamilyIndex);

    // Make sure we got queue index from the above function.
    assert (queueFamilyIndex != UINT32_MAX);

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
                         /*.uint32_t.....................graphicsQueueIndex...........*/ queueFamilyIndex,
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
     
    std::string currentPath   = std::filesystem::current_path ().string ();
    std::string assetsDirPath = currentPath + std::string ("\\..\\..\\..\\Assets");
    std::string modelPath     = assetsDirPath + std::string ("\\Models\\simple_triangle.obj");
    assert (modelPath.length() > 0); //adding this just to remind myself to add an actual path above.
    printf ("model path = %s\n", modelPath.c_str ());

    const aiScene*     pScene           = aiImportFile (modelPath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
    GeometryBufferSet  geometrysBuffers = {}; // Initializing the struct to 0 so any non-0 values are indicative of one of the variables being changed.

    // Create a vertex and index buffer
    geometrysBuffers = CreateGeometryBuffersFromAiScene (physicalDevice,
                                                         logicalDevice,
                                                         queue,
                                                         queueFamilyIndex,
                                                         pScene,
                                                         true);
   

    uint32_t numFramesToRender = 5;
    
    printf("about to start executing renderloop\n");
    for (uint32_t i = 0; i < numFramesToRender; i++)
    {
        ExecuteRenderLoop (/*.VkDevice.....................logicalDevice................*/ logicalDevice,
                            /*.VkPhysicalDevice.............physicalDevice,..............*/ physicalDevice,
                            /*.VkSwapchainKHR...............swapchain....................*/ swapchain,
                            /*.VkQueue......................queue........................*/ queue,
                            /*.uint32_t.....................gfxQueueIdx..................*/ queueFamilyIndex,
                            /*.uint32_t.....................numPreferredSwapchainFormats.*/ numPreferredSurfaceFormats,
                            /*.VkFormat*....................pPreferredSwapchainFormats...*/ pPreferredSurfaceFormats,
                            /*.VkSurfaceKHR.................surface......................*/ surface,
                            /*.VkRenderPass.................renderpass...................*/ renderpass,
                            /*.VkPipeline...................pipeline.....................*/ pipeline,
                            /*.PerSwapchainImageResources**.ppPerSwapchainImageResources.*/ &pPerSwapchainImageResources,
                            /*.uint32_t*....................pNumSwapchainImages..........*/ &numSwapChainImages,
                            /*.VkExtent2D*..................pFramebufferExtent...........*/ &actualFrameDimensions,
                            /*.GeometryBufferSet*...........vertexBuffer.................*/ &geometrysBuffers,
                            /*.uint32_t.....................frameIdx.....................*/ i                     );
    }
   
    printf ("finished exedcuting render loop %u times.\n", numFramesToRender);

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
int main ()
 {
    return wWinMain (GetModuleHandle (NULL), NULL, GetCommandLineW (), SW_SHOWNORMAL);
 }
