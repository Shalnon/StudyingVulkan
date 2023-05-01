// StudyingVulkan.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "StudyingVulkan.h"
#include "Vulkan_Utils.h"
#include "AppWindowingSystem.h"
#include "resource.h"
#include "Vulkan_Synchronization.h"

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

    std::string vertexShaderPath   = std::string(VERT_PATH);
    std::string fragmentShaderPath = std::string(FRAG_PATH);

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

    VkPipeline pipeline = CreatePipeline(logicalDevice, 
                                         renderpass,
                                         &actualFrameDimensions,
                                         fragmentShaderPath.c_str(),
                                         vertexShaderPath.c_str());

    CreateFrameBuffers(logicalDevice,
                       renderpass,
                       &actualFrameDimensions,
                       numSwapChainImages,
                       pPerSwapchainImageResources);

    uint32_t numFramesToRender = 3;
    
    printf("executed renderloop\n");
    for (uint32_t i = 0; i < numFramesToRender; i++)
    {
        ExecuteRenderLoop (/*.VkDevice.....................logicalDevice................*/ logicalDevice,
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
                            /*.uint32_t.....................frameIdx.....................*/ i                     );
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
