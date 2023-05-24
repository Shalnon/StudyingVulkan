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

    VkRenderPass          renderpass                = CreateRenderpass (surfaceFormat.format, logicalDevice);

    VkDescriptorSetLayout descriptorSetLayoutHandle = CreateDescriptorSetLayout (logicalDevice);
    VkDescriptorPool      descriptorPoolHandle      = CreateDescriptorPool (logicalDevice); //@Note: Dont actually need the descriptorPool till we allocate descriptorSets below

    VkPipelineLayout      pipelineLayoutHandle      = VK_NULL_HANDLE;
    VkPipeline            pipelineHandle            = CreatePipeline(logicalDevice, 
                                                                     renderpass,
                                                                     &actualFrameDimensions,
                                                                     descriptorSetLayoutHandle,
                                                                     fragmentShaderPath.c_str(),
                                                                     vertexShaderPath.c_str(),
                                                                     &pipelineLayoutHandle);

    CreateFrameBuffers(logicalDevice,
                       renderpass,
                       &actualFrameDimensions,
                       numSwapChainImages,
                       pPerSwapchainImageResources);
     
    std::string currentPath   = std::filesystem::current_path ().string ();
    std::string assetsDirPath = currentPath   + std::string ("\\..\\..\\..\\Assets");
    std::string modelPath     = assetsDirPath + std::string ("\\Models\\simple_triangle.obj");

    printf ("model path = %s\n", modelPath.c_str ());

    VkAabbPositionsKHR sceneBounds =
    {
        /*...float....minX...*/ -0.15f,
        /*...float....minY...*/ -0.15f,
        /*...float....minZ...*/ -0.15f,
        /*...float....maxX...*/  0.15f,
        /*...float....maxY...*/  0.15f,
        /*...float....maxZ...*/  0.15f
    };

    //glm::vec3 sceneBounds 
    const aiScene*            pScene            = aiImportFile (modelPath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
    glm::vec3                 sceneScale        = { 1.0f, 1.0f, 1.0f };
    MeshInfo*                 pMeshes           = nullptr;

    // Create a vertex and index buffer
    GeometryBufferSet  geometrysBuffers = CreateGeometryBuffersAndAABBs (physicalDevice,
                                                                         logicalDevice,
                                                                         queue,
                                                                         queueFamilyIndex,
                                                                         pScene);
    // Creates UBO and fills it with data. Contains scene transform
    vulkanAllocatedBufferInfo uniformBufferInfo  = CreateUniformBuffer(physicalDevice,
                                                                       logicalDevice,
                                                                       queue,
                                                                       queueFamilyIndex,
                                                                       &geometrysBuffers,
                                                                       &sceneBounds,
                                                                       true);

    VkDescriptorBufferInfo transformBufferDescriptorInfo =
    {
        /*...VkBuffer........buffer...*/ uniformBufferInfo.bufferHandle,
        /*...VkDeviceSize....offset...*/ 0,
        /*...VkDeviceSize....range....*/ VK_WHOLE_SIZE
    };

    // Fills out a VkDescriptorSetAllocateInfo and calls vkAllocateDescriptorSets
    VkDescriptorSet descriptorSetHandle = AllocateDescriptorSet (logicalDevice,
                                                                 descriptorPoolHandle,
                                                                 descriptorSetLayoutHandle);

    //@TODO: change name of this func so it makes it clear its updating the desciptor set, not the transform data in the buffer itself.
    // Fills out a VkWriteDescriptorSet and calls vkUpdateDescriptorSets() 
    UpdateMeshTransformUbo (logicalDevice,
                            &transformBufferDescriptorInfo,
                            descriptorSetHandle);

    uint32_t numFramesToRender = 5;
    
    printf("about to start executing renderloop\n");
    for (uint32_t i = 0; i < numFramesToRender; i++)
    {
        printf ("\n--Frame %u begin--\n", i);
        ExecuteRenderLoop ( /*.VkDevice.....................logicalDevice................*/ logicalDevice,
                            /*.VkPhysicalDevice.............physicalDevice,..............*/ physicalDevice,
                            /*.VkSwapchainKHR...............swapchain....................*/ swapchain,
                            /*VkDescriptorSet...............descriptorSetHandle..........*/ descriptorSetHandle,
                            /*.VkQueue......................queue........................*/ queue,
                            /*.uint32_t.....................gfxQueueIdx..................*/ queueFamilyIndex,
                            /*.uint32_t.....................numPreferredSwapchainFormats.*/ numPreferredSurfaceFormats,
                            /*.VkFormat*....................pPreferredSwapchainFormats...*/ pPreferredSurfaceFormats,
                            /*.VkSurfaceKHR.................surface......................*/ surface,
                            /*.VkRenderPass.................renderpass...................*/ renderpass,
                            /*.VkPipeline...................pipelineHandle...............*/ pipelineHandle,
                            /*.VkPipelineLayout.............pipelineLayoutHandle.........*/ pipelineLayoutHandle,
                            /*.PerSwapchainImageResources**.ppPerSwapchainImageResources.*/ &pPerSwapchainImageResources,
                            /*.uint32_t*....................pNumSwapchainImages..........*/ &numSwapChainImages,
                            /*.VkExtent2D*..................pFramebufferExtent...........*/ &actualFrameDimensions,
                            /*.GeometryBufferSet*...........vertexBuffer.................*/ &geometrysBuffers,
                            /*.uint32_t.....................frameIdx.....................*/ i                     );
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

        if (pipelineHandle != VK_NULL_HANDLE)
        {
            vkDestroyPipeline (logicalDevice, pipelineHandle, nullptr);
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
