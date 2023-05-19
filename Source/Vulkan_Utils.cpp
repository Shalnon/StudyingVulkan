#define VOLK_IMPLEMENTATION

#include "Vulkan_Utils.h"
#include "Vulkan_enum_strings.h"
#include "Vulkan_Buffer_Tools.h"
uint32_t numRequiredExtensions = 1;



    uint32_t g_FragShader[] =
    { 0x07230203,0x00010600,0x000d000a,0x00000013,
    0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,
    0x00000000,0x0003000e,0x00000000,0x00000001,
    0x0007000f,0x00000004,0x00000004,0x6e69616d,
    0x00000000,0x00000009,0x0000000c,0x00030010,
    0x00000004,0x00000007,0x00030003,0x00000002,
    0x000001a4,0x000a0004,0x475f4c47,0x4c474f4f,
    0x70635f45,0x74735f70,0x5f656c79,0x656e696c,
    0x7269645f,0x69746365,0x00006576,0x00080004,
    0x475f4c47,0x4c474f4f,0x6e695f45,0x64756c63,
    0x69645f65,0x74636572,0x00657669,0x00040005,
    0x00000004,0x6e69616d,0x00000000,0x00050005,
    0x00000009,0x5f74756f,0x6f6c6f63,0x00000072,
    0x00050005,0x0000000c,0x635f6e69,0x726f6c6f,
    0x00000000,0x00030047,0x00000009,0x00000000,
    0x00040047,0x00000009,0x0000001e,0x00000000,
    0x00030047,0x0000000c,0x00000000,0x00040047,
    0x0000000c,0x0000001e,0x00000000,0x00030047,
    0x0000000d,0x00000000,0x00030047,0x0000000f,
    0x00000000,0x00030047,0x00000010,0x00000000,
    0x00030047,0x00000011,0x00000000,0x00030047,
    0x00000012,0x00000000,0x00020013,0x00000002,
    0x00030021,0x00000003,0x00000002,0x00030016,
    0x00000006,0x00000020,0x00040017,0x00000007,
    0x00000006,0x00000004,0x00040020,0x00000008,
    0x00000003,0x00000007,0x0004003b,0x00000008,
    0x00000009,0x00000003,0x00040017,0x0000000a,
    0x00000006,0x00000003,0x00040020,0x0000000b,
    0x00000001,0x0000000a,0x0004003b,0x0000000b,
    0x0000000c,0x00000001,0x0004002b,0x00000006,
    0x0000000e,0x3f800000,0x00050036,0x00000002,
    0x00000004,0x00000000,0x00000003,0x000200f8,
    0x00000005,0x0004003d,0x0000000a,0x0000000d,
    0x0000000c,0x00050051,0x00000006,0x0000000f,
    0x0000000d,0x00000000,0x00050051,0x00000006,
    0x00000010,0x0000000d,0x00000001,0x00050051,
    0x00000006,0x00000011,0x0000000d,0x00000002,
    0x00070050,0x00000007,0x00000012,0x0000000f,
    0x00000010,0x00000011,0x0000000e,0x0003003e,
    0x00000009,0x00000012,0x000100fd,0x00010038
    };

    uint32_t g_VertShader[] =
    { 0x07230203,0x00010600,0x000d000a,0x00000036,
    0x00000000,0x00020011,0x00000001,0x0006000b,
    0x00000001,0x4c534c47,0x6474732e,0x3035342e,
    0x00000000,0x0003000e,0x00000000,0x00000001,
    0x000a000f,0x00000000,0x00000004,0x6e69616d,
    0x00000000,0x0000000c,0x00000016,0x00000022,
    0x00000026,0x00000031,0x00030003,0x00000002,
    0x000001a4,0x000a0004,0x475f4c47,0x4c474f4f,
    0x70635f45,0x74735f70,0x5f656c79,0x656e696c,
    0x7269645f,0x69746365,0x00006576,0x00080004,
    0x475f4c47,0x4c474f4f,0x6e695f45,0x64756c63,
    0x69645f65,0x74636572,0x00657669,0x00040005,
    0x00000004,0x6e69616d,0x00000000,0x00070005,
    0x0000000c,0x61697274,0x656c676e,0x736f705f,
    0x6f697469,0x0000736e,0x00060005,0x00000016,
    0x61697274,0x656c676e,0x6c6f635f,0x0073726f,
    0x00060005,0x00000020,0x505f6c67,0x65567265,
    0x78657472,0x00000000,0x00060006,0x00000020,
    0x00000000,0x505f6c67,0x7469736f,0x006e6f69,
    0x00070006,0x00000020,0x00000001,0x505f6c67,
    0x746e696f,0x657a6953,0x00000000,0x00070006,
    0x00000020,0x00000002,0x435f6c67,0x4470696c,
    0x61747369,0x0065636e,0x00030005,0x00000022,
    0x00000000,0x00060005,0x00000026,0x565f6c67,
    0x65747265,0x646e4978,0x00007865,0x00050005,
    0x00000031,0x5f74756f,0x6f6c6f63,0x00000072,
    0x00030047,0x0000000c,0x00000000,0x00030047,
    0x00000016,0x00000000,0x00050048,0x00000020,
    0x00000000,0x0000000b,0x00000000,0x00050048,
    0x00000020,0x00000001,0x0000000b,0x00000001,
    0x00050048,0x00000020,0x00000002,0x0000000b,
    0x00000003,0x00030047,0x00000020,0x00000002,
    0x00040047,0x00000026,0x0000000b,0x0000002a,
    0x00030047,0x0000002a,0x00000000,0x00030047,
    0x0000002b,0x00000000,0x00030047,0x0000002c,
    0x00000000,0x00030047,0x0000002d,0x00000000,
    0x00030047,0x00000031,0x00000000,0x00040047,
    0x00000031,0x0000001e,0x00000000,0x00030047,
    0x00000035,0x00000000,0x00020013,0x00000002,
    0x00030021,0x00000003,0x00000002,0x00030016,
    0x00000006,0x00000020,0x00040017,0x00000007,
    0x00000006,0x00000002,0x00040015,0x00000008,
    0x00000020,0x00000000,0x0004002b,0x00000008,
    0x00000009,0x00000003,0x0004001c,0x0000000a,
    0x00000007,0x00000009,0x00040020,0x0000000b,
    0x00000006,0x0000000a,0x0004003b,0x0000000b,
    0x0000000c,0x00000006,0x0004002b,0x00000006,
    0x0000000d,0x3f000000,0x0004002b,0x00000006,
    0x0000000e,0xbf000000,0x0005002c,0x00000007,
    0x0000000f,0x0000000d,0x0000000e,0x0005002c,
    0x00000007,0x00000010,0x0000000d,0x0000000d,
    0x0005002c,0x00000007,0x00000011,0x0000000e,
    0x0000000d,0x0006002c,0x0000000a,0x00000012,
    0x0000000f,0x00000010,0x00000011,0x00040017,
    0x00000013,0x00000006,0x00000003,0x0004001c,
    0x00000014,0x00000013,0x00000009,0x00040020,
    0x00000015,0x00000006,0x00000014,0x0004003b,
    0x00000015,0x00000016,0x00000006,0x0004002b,
    0x00000006,0x00000017,0x3f800000,0x0004002b,
    0x00000006,0x00000018,0x00000000,0x0006002c,
    0x00000013,0x00000019,0x00000017,0x00000018,
    0x00000018,0x0006002c,0x00000013,0x0000001a,
    0x00000018,0x00000017,0x00000018,0x0006002c,
    0x00000013,0x0000001b,0x00000018,0x00000018,
    0x00000017,0x0006002c,0x00000014,0x0000001c,
    0x00000019,0x0000001a,0x0000001b,0x00040017,
    0x0000001d,0x00000006,0x00000004,0x0004002b,
    0x00000008,0x0000001e,0x00000001,0x0004001c,
    0x0000001f,0x00000006,0x0000001e,0x0005001e,
    0x00000020,0x0000001d,0x00000006,0x0000001f,
    0x00040020,0x00000021,0x00000003,0x00000020,
    0x0004003b,0x00000021,0x00000022,0x00000003,
    0x00040015,0x00000023,0x00000020,0x00000001,
    0x0004002b,0x00000023,0x00000024,0x00000000,
    0x00040020,0x00000025,0x00000001,0x00000023,
    0x0004003b,0x00000025,0x00000026,0x00000001,
    0x00040020,0x00000028,0x00000006,0x00000007,
    0x00040020,0x0000002e,0x00000003,0x0000001d,
    0x00040020,0x00000030,0x00000003,0x00000013,
    0x0004003b,0x00000030,0x00000031,0x00000003,
    0x00040020,0x00000033,0x00000006,0x00000013,
    0x00050036,0x00000002,0x00000004,0x00000000,
    0x00000003,0x000200f8,0x00000005,0x0003003e,
    0x0000000c,0x00000012,0x0003003e,0x00000016,
    0x0000001c,0x0004003d,0x00000023,0x00000027,
    0x00000026,0x00050041,0x00000028,0x00000029,
    0x0000000c,0x00000027,0x0004003d,0x00000007,
    0x0000002a,0x00000029,0x00050051,0x00000006,
    0x0000002b,0x0000002a,0x00000000,0x00050051,
    0x00000006,0x0000002c,0x0000002a,0x00000001,
    0x00070050,0x0000001d,0x0000002d,0x0000002b,
    0x0000002c,0x00000018,0x00000017,0x00050041,
    0x0000002e,0x0000002f,0x00000022,0x00000024,
    0x0003003e,0x0000002f,0x0000002d,0x0004003d,
    0x00000023,0x00000032,0x00000026,0x00050041,
    0x00000033,0x00000034,0x00000016,0x00000032,
    0x0004003d,0x00000013,0x00000035,0x00000034,
    0x0003003e,0x00000031,0x00000035,0x000100fd,
    0x00010038
    };

// Static const struct member value assignments:
const VkFormat const SwapchainInfo::pPreferredSwapchainFormats[NUM_PREFERRED_SURFACE_FORMATS] = PREFERRED_SURRFACE_FORMATS;



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
        uint32_t           numLayers = 0;
        const char* const* ppEnabledLayerNames = 0;
        const char*        ppExtensionNames[]      = { vkKhrSurfaceExtensionName , win32SurfaceExtensionName };
        VkApplicationInfo  applicationInfo =
        {
            /*VkStructureType....sType......................*/ VK_STRUCTURE_TYPE_APPLICATION_INFO,
            /*const.void*........pNext......................*/ 0,
            /*const.char*........pApplicationName...........*/ "StudyingVulkan",
            /*uint32_t...........applicationVersion.........*/ 0,
            /*const.char*........pEngineName................*/ "TheFuckYouEngine",
            /*uint32_t...........engineVersion..............*/ 0,
            /*uint32_t...........apiVersion.................*/ VK_API_VERSION_1_3
        };

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
   // g_vkEnumeratePhysicalDevices                   =    reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
   // g_vkGetPhysicalDeviceQueueFamilyProperties     =    reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties"));
   // g_vkGetPhysicalDeviceSurfaceFormatsKHR         =    reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
   // g_vkGetSwapchainImagesKHR                      =    reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetInstanceProcAddr(instance, "vkGetSwapchainImagesKHR"));
   // g_vkCreateWin32SurfaceKHR                      =    reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR"));
   // g_vkEnumeratePhysicalDevices                   =    reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
   // g_vkGetPhysicalDeviceQueueFamilyProperties     =    reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties"));
   // g_vkGetPhysicalDeviceSurfaceSupportKHR         =    reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
   // g_vkCreateDevice                               =    reinterpret_cast<PFN_vkCreateDevice>(vkGetInstanceProcAddr(instance, "vkCreateDevice"));
   // g_vkGetDeviceQueue                             =    reinterpret_cast<PFN_vkGetDeviceQueue>(vkGetInstanceProcAddr(instance, "vkGetDeviceQueue"));
   // g_vkGetPhysicalDeviceSurfaceCapabilitiesKHR    =    reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
   // g_vkGetPhysicalDeviceSurfaceFormatsKHR         =    reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
   // g_vkCreateSwapchainKHR                         =    reinterpret_cast<PFN_vkCreateSwapchainKHR>(vkGetInstanceProcAddr(instance, "vkCreateSwapchainKHR"));
   // g_vkGetSwapchainImagesKHR                      =    reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetInstanceProcAddr(instance, "vkGetSwapchainImagesKHR"));
   // g_vkCreateFence                                =    reinterpret_cast<PFN_vkCreateFence>(vkGetInstanceProcAddr(instance, "vkCreateFence"));
   // g_vkCreateCommandPool                          =    reinterpret_cast<PFN_vkCreateCommandPool>(vkGetInstanceProcAddr(instance, "vkCreateCommandPool"));
   // g_vkAllocateCommandBuffers                     =    reinterpret_cast<PFN_vkAllocateCommandBuffers>(vkGetInstanceProcAddr(instance, "vkAllocateCommandBuffers"));
   // g_vkCreateImageView                            =    reinterpret_cast<PFN_vkCreateImageView>(vkGetInstanceProcAddr(instance, "vkCreateImageView"));
   // g_vkCreateRenderPass                           =    reinterpret_cast<PFN_vkCreateRenderPass>(vkGetInstanceProcAddr(instance, "vkCreateRenderPass"));
   // g_vkCreateShaderModule                         =    reinterpret_cast<PFN_vkCreateShaderModule>(vkGetInstanceProcAddr(instance, "vkCreateShaderModule"));
   // g_vkCreatePipelineLayout                       =    reinterpret_cast<PFN_vkCreatePipelineLayout>(vkGetInstanceProcAddr(instance, "vkCreatePipelineLayout"));
   // g_vkCreateGraphicsPipelines                    =    reinterpret_cast<PFN_vkCreateGraphicsPipelines>(vkGetInstanceProcAddr(instance, "vkCreateGraphicsPipelines"));
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
                                  char**                    ppRequiredDeviceExtensionNames,
                                  VkPhysicalDeviceFeatures* pDeviceFeatures,  //get rid of this arg
                                  VkPhysicalDevice*         pPhysicalDeviceOut,
                                  VkQueue*                  pQueueOut,
                                  VkDevice*                 pLogicalDeviceOut,
                                  uint32_t*                 pQueueIndexOut)
{
    VkResult         result;

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

    VkPhysicalDeviceFeatures  physicalDeviceFeatures    = {};
    VkPhysicalDeviceProperties physicalDeviceProperties = {};

    while ((FoundSuitableQueueDeviceCombo == false) && (deviceIdx < deviceCount))
    {
        VkPhysicalDevice         physicalDevice           = pPhysicalDevices[deviceIdx];

        physicalDeviceFeatures = {};
        physicalDeviceProperties = {};

        vkGetPhysicalDeviceFeatures(*pPhysicalDevices, &physicalDeviceFeatures);
        vkGetPhysicalDeviceProperties(*pPhysicalDevices, &physicalDeviceProperties);

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

    VkDeviceCreateInfo deviceCreateInfo =
    {
        /*....VkStructureType....................sType.......................*/ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        /*....const.void*........................pNext.......................*/ 0,
        /*....VkDeviceCreateFlags................flags.......................*/ 0,
        /*....uint32_t...........................queueCreateInfoCount........*/ 1,
        /*....const.VkDeviceQueueCreateInfo*.....pQueueCreateInfos...........*/ &queueInfo,
        /*....uint32_t...........................enabledLayerCount...........*/ 0,
        /*....const.char*.const*.................ppEnabledLayerNames.........*/ 0,
        /*....uint32_t...........................enabledExtensionCount.......*/ numnRequiredExtensions,
        /*....const.char*.const*.................ppEnabledExtensionNames.....*/ ppRequiredDeviceExtensionNames,
        /*....const.VkPhysicalDeviceFeatures*....pEnabledFeatures............*/ 0//&physicalDeviceFeatures
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
                         VkFormat*                    pPreferredSurfaceFormats,
                         VkExtent2D                   prefferredExtent,           // Dimensions of the window
                         VkSwapchainKHR               oldSwapchain,
                         VkSwapchainKHR*              pSwapchainOut,
                         VkExtent2D*                  pSwapchainExtentChosenOut,  // Swapchain format used. see related note in function body
                         VkSurfaceFormatKHR*          pSurfaceFormatOut,          // pass back the format used
                         uint32_t*                    pNumSwapchainImages,
                         PerSwapchainImageResources** ppPerSwapchainImageResources)
{
    // Get Surface capabilities
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};
    VkResult                 result = VK_SUCCESS;

    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);
    assert(result == VK_SUCCESS);
    assert(surfaceCapabilities.maxImageCount > 0);


    //Choose a surface format to use with swapchain
    uint32_t                 supportedFormatCount                                 = GetPhysicalDeviceSurfaceFormatCount(physicalDevice, surface);
    VkSurfaceFormatKHR*      pSupportedSwapchainSurfaceFormats                    = reinterpret_cast<VkSurfaceFormatKHR*>(calloc(sizeof(VkSurfaceFormatKHR), supportedFormatCount));

    //Get supported swapchain formats
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &supportedFormatCount, pSupportedSwapchainSurfaceFormats);

    //Choose a swapchain format
    VkSurfaceFormatKHR chosenSwapchainFormat = ChooseSwapchainFormat(pSupportedSwapchainSurfaceFormats, supportedFormatCount, pPreferredSurfaceFormats, numPreferredSurfaceFormats);

    ///@Note: The spec says: "currentExtent is the current width and height of the surface, or the special value (0xFFFFFFFF, 0xFFFFFFFF) indicating that the surface size will be determined by the extent of a swapchain targeting the surface."
    VkExtent2D swapchainExtent = (surfaceCapabilities.currentExtent.height == 0xFFFFFFFF) ? prefferredExtent : surfaceCapabilities.currentExtent;

    // Saving this for later
    *pSwapchainExtentChosenOut = swapchainExtent;
    *pSurfaceFormatOut         = chosenSwapchainFormat;

    // Figure out how many images to use with the swapchain. Make sure we dont use more than the max.
    uint32_t preferredNumSwapChainImages = surfaceCapabilities.minImageCount + 1;
    uint32_t actualNumSwapchainImages    = (preferredNumSwapChainImages <= surfaceCapabilities.maxImageCount) ? 
                                                                               preferredNumSwapChainImages : surfaceCapabilities.maxImageCount;

    // Choose a surface transform
    VkSurfaceTransformFlagBitsKHR preferredSurfaceTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    VkSurfaceTransformFlagBitsKHR preTransform =
        ((surfaceCapabilities.supportedTransforms & preferredSurfaceTransform) > 0) ? preferredSurfaceTransform : surfaceCapabilities.currentTransform;

    //@TODO: Should look into what to do if VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR isnt supported. that might also require changes to the FS output. 
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
        /*....con st.uint32_t*..................pQueueFamilyIndices.....*/ 0,
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

    PerSwapchainImageResources* pPerSwapchainImageResources = *ppPerSwapchainImageResources;

    if (pPerSwapchainImageResources != nullptr)
    {
        ResestPerSwapchainImageResources (logicalDevice, pPerSwapchainImageResources, *pNumSwapchainImages);
    }

    if (*pNumSwapchainImages != imageCount)
    {
        free (pPerSwapchainImageResources);
        pPerSwapchainImageResources = static_cast<PerSwapchainImageResources*>(calloc(imageCount, sizeof(PerSwapchainImageResources)));
    }


     

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

        printf("Created fence(0x%016x) for swapchain image #%u, initial status is %s\n", 
                       pPerSwapchainImageResources[i].queueSubmitFence,
                       i,
                       VkResultToString (vkGetFenceStatus (logicalDevice, pPerSwapchainImageResources[i].queueSubmitFence)));

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


    // Create image views for each of  the swapchain images
    for (uint32_t imageIdx = 0; imageIdx < imageCount; imageIdx++)
    {

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
            /*...VkImage....................image...............*/ pSwapchainImages[imageIdx],
            /*...VkImageViewType............viewType............*/ VK_IMAGE_VIEW_TYPE_2D,
            /*...VkFormat...................format..............*/ chosenSwapchainFormat.format,
            /*...VkComponentMapping.........components..........*/ { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A},
            /*...VkImageSubresourceRange....subresourceRange....*/ subResourceRange
        };
        result = vkCreateImageView(logicalDevice, &imageViewCreateInfo, 0, &(pPerSwapchainImageResources[imageIdx].imageView));
        assert(result == VK_SUCCESS);
    }

    *ppPerSwapchainImageResources = pPerSwapchainImageResources;
    *pNumSwapchainImages          = imageCount;
    *pSwapchainOut                = swapchain;
}

VkSurfaceFormatKHR ChooseSwapchainFormat(VkSurfaceFormatKHR* pSupportedSwapchainSurfaceFormats, uint32_t numSupportedFormats, VkFormat* pPreferredFormats, uint32_t numPreferredFormats)
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

VkRenderPass CreateRenderpass(VkFormat swapChainFormat, VkDevice logicalDevice)
{
    VkAttachmentDescription attachmentDescription =
    {
        /*..VkAttachmentDescriptionFlags....flags............*/ 0,
        /*..VkFormat........................format...........*/ swapChainFormat,
        /*..VkSampleCountFlagBits...........samples..........*/ VK_SAMPLE_COUNT_1_BIT,
        /*..VkAttachmentLoadOp..............loadOp...........*/ VK_ATTACHMENT_LOAD_OP_CLEAR,
        /*..VkAttachmentStoreOp.............storeOp..........*/ VK_ATTACHMENT_STORE_OP_STORE,
        /*..VkAttachmentLoadOp..............stencilLoadOp....*/ VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // Not using stencil so dont care
        /*..VkAttachmentStoreOp.............stencilStoreOp...*/ VK_ATTACHMENT_STORE_OP_DONT_CARE, // Not using stencil so dont care
        /*..VkImageLayout...................initialLayout....*/ VK_IMAGE_LAYOUT_UNDEFINED,
        /*..VkImageLayout...................finalLayout......*/ VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentReference colorAttachmentRef =
    {
        /*...uint32_t.........attachment....*/ 0,
        /*...VkImageLayout....layout........*/ VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    // From khronos sample:
    // "|We will end up with two transitions.                                         |
    //  |The first one happens right before we start subpass #0, where                |
    //  |UNDEFINED is transitioned into COLOR_ATTACHMENT_OPTIMAL.                     |
    //  |The final layout in the render pass attachment states PRESENT_SRC_KHR, so we |
    //  |will get a final transition from COLOR_ATTACHMENT_OPTIMAL to PRESENT_SRC_KHR.|"
    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency subpassDependency =
    {
        /*....uint32_t................srcSubpass..........*/ VK_SUBPASS_EXTERNAL,
        /*....uint32_t................dstSubpass..........*/ 0,
        /*....VkPipelineStageFlags....srcStageMask........*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        /*....VkPipelineStageFlags....dstStageMask........*/ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        /*....VkAccessFlags...........srcAccessMask.......*/ 0, // From khronos sample: "Since we changed the image layout, we need to make the memory visible to color attachment to modify."
        /*....VkAccessFlags...........dstAccessMask.......*/ VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        /*....VkDependencyFlags.......dependencyFlags.....*/ 0
    };

    VkRenderPassCreateInfo renderPassCreateInfo =
    {
        /*...VkStructureType...................sType.............*/ VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        /*...const.void*.......................pNext.............*/ 0,
        /*...VkRenderPassCreateFlags...........flags.............*/ 0,
        /*...uint32_t..........................attachmentCount...*/ 1,
        /*...const.VkAttachmentDescription*....pAttachments......*/ &attachmentDescription,
        /*...uint32_t..........................subpassCount......*/ 1,
        /*...const.VkSubpassDescription*.......pSubpasses........*/ &subpassDescription,
        /*...uint32_t..........................dependencyCount...*/ 1,
        /*...const.VkSubpassDependency*........pDependencies.....*/ &subpassDependency
    };

    VkRenderPass renderpass = VK_NULL_HANDLE;
    VkResult result = vkCreateRenderPass(logicalDevice, &renderPassCreateInfo, 0, &renderpass);
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

    uint64_t  rawBaseAddr = reinterpret_cast<uint64_t>(pAllocation);
    uint64_t  remainder   = rawBaseAddr % baseAddrAlignment; //this probably doesnt need to be uint64_t. probs could even just use an uint8_t
    uint64_t  alignedBaseAddr = rawBaseAddr + remainder;

    AlignedAllocation alignedAlloc      = {};
    alignedAlloc.pRawAllocatiotBaseAddr = static_cast<uint8_t*>(pAllocation);
    alignedAlloc.alignedBaseAddress     = reinterpret_cast<uint8_t*>(alignedBaseAddr);
    alignedAlloc.alignedSize             = (rawBaseAddr + rawAllocSize) - alignedBaseAddr; // subtract aligned base addr, from addr of last byte in allocation

    return alignedAlloc;
}*/

/// <summary>
/// Tales a VkDevice and a path to a binary .spv file and uses it to create a module.
/// </summary>
/// <param name="logicalDevice">- A VkDevice</param>
/// <param name="spvPath">- binary .spv file. likely produced via glslc</param>
/// <returns>A newly created shader module based off of the .spv</returns>
VkShaderModule CreateShaderModule(VkDevice logicalDevice, const char* spvPath, bool isFrag, bool isVert)
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

    /*const uint32_t* pCode = 0;
    size_t  codeSizeB = 0;
    
    if (isVert == true)
    {
        pCode = g_VertShader;
        codeSizeB = sizeof(g_VertShader);
    }
    else if (isFrag == true)
    {
        pCode = g_FragShader;
        codeSizeB = sizeof(g_FragShader);
    }*/

    VkShaderModule           shaderModule           = VK_NULL_HANDLE;
    VkShaderModuleCreateInfo shaderModuleCreateInfo =
    {
        /*...VkStructureType..............sType.......*/ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        /*...const.void*..................pNext.......*/ 0,
        /*...VkShaderModuleCreateFlags....flags.......*/ 0,
        /*...size_t.......................codeSize....*/ codeSize, // The validation layer seems to be using this as like how many ptrs ?
        /*...const.uint32_t*..............pCode.......*/ pCode// reinterpret_cast<uint32_t*>(spvCode.alignedBaseAddress)
    };
    printf("codeSize = %llu, codesize / 4 = %llu\n", codeSize, codeSize / 4);
    printf("fromStruct: codeSize = %llu, codesize / 4 = %llu\n", shaderModuleCreateInfo.codeSize, shaderModuleCreateInfo.codeSize / 4);
    VkResult result = vkCreateShaderModule(logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);
    assert(result == VK_SUCCESS);
    assert(shaderModule != VK_NULL_HANDLE);

    return shaderModule;
}


VkPipeline CreatePipeline(VkDevice logicalDevice, VkRenderPass renderpass, VkExtent2D* pExtent, const char* fragShaderPath, const char* vertShaderPath)
{
    uint32_t numVertexAttributes = sizeof (s_VertexShaderAttributes) / sizeof (AttributeInfo);
    assert (numVertexAttributes > 0);
 
    VkVertexInputAttributeDescription inputAttributeDescription =
    {
        /*...uint32_t....location....*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].attributeIdx,
        /*...uint32_t....binding.....*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].bufferBindingIdx,
        /*...VkFormat....format......*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].format,
        /*...uint32_t....offset......*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].offset
    };

    // Position binding
    VkVertexInputBindingDescription       vertexInputDescription =
    {
        /*..uint32_t.............binding......*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].bufferBindingIdx,
        /*..uint32_t.............stride.......*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].stride,
        /*..VkVertexInputRate....inputRate....*/ VK_VERTEX_INPUT_RATE_VERTEX
    };

    // The pipeline layout is used to register resources like samplers, SSBOs and other resources requiring a descriptor set to be used
    VkPipelineLayout                       pipelineLayout             = VK_NULL_HANDLE;
    VkPipelineVertexInputStateCreateInfo   vertexInputStateCreateInfo =
    {
        /*...VkStructureType.............................sType.............................*/ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        /*...const.void*.................................pNext.............................*/ nullptr,
        /*...VkPipelineVertexInputStateCreateFlags.......flags.............................*/ 0, // Reserved for future use as of vulkan 1.3
        /*...uint32_t....................................vertexBindingDescriptionCount.....*/ 1,
        /*...const.VkVertexInputBindingDescription*......pVertexBindingDescriptions........*/ &vertexInputDescription,
        /*...uint32_t....................................vertexAttributeDescriptionCount...*/ numVertexAttributes,
        /*...const.VkVertexInputAttributeDescription*....pVertexAttributeDescriptions......*/ &inputAttributeDescription
    };

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo =
    {
        /*....VkStructureType............................sType......................*/ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        /*....const.void*................................pNext......................*/ 0, 
        /*....VkPipelineInputAssemblyStateCreateFlags....flags......................*/ 0,
        /*....VkPrimitiveTopology........................topology...................*/ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        /*....VkBool32...................................primitiveRestartEnable.....*/ VK_FALSE
    };

    VkPipelineLayoutCreateInfo             layoutCreateInfo =
    {
        /*...VkStructureType.................sType......................*/ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        /*...const.void*.....................pNext......................*/ nullptr,
        /*...VkPipelineLayoutCreateFlags.....flags......................*/ 0,
        /*...uint32_t........................setLayoutCount.............*/ 0,
        /*...const.VkDescriptorSetLayout*....pSetLayouts................*/ nullptr,
        /*...uint32_t........................pushConstantRangeCount.....*/ 0,
        /*...const.VkPushConstantRange*......pPushConstantRanges........*/ nullptr
    };

    //@VKSPEC: "Access to descriptor sets from a pipeline is accomplished through a pipeline layout." : https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPipelineLayout.html
    VkResult result = vkCreatePipelineLayout(logicalDevice, &layoutCreateInfo, 0, &pipelineLayout);
    assert(result == VK_SUCCESS);

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo =
    {
        /*...VkStructureType............................sType......................*/ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        /*...const.void*................................pNext......................*/ 0,
        /*...VkPipelineRasterizationStateCreateFlags....flags......................*/ 0,
        /*...VkBool32...................................depthClampEnable...........*/ VK_FALSE,
        /*...VkBool32...................................rasterizerDiscardEnable....*/ VK_FALSE,
        /*...VkPolygonMode..............................polygonMode................*/ VK_POLYGON_MODE_FILL, //VK_POLYGON_MODE_FILL  = 0, so dont actually need to explicitly set it technically.
        /*...VkCullModeFlags............................cullMode...................*/ VK_CULL_MODE_BACK_BIT,
        /*...VkFrontFace................................frontFace..................*/ VK_FRONT_FACE_CLOCKWISE,
        /*...VkBool32...................................depthBiasEnable............*/ VK_FALSE,
        /*...float......................................depthBiasConstantFactor....*/ 0.0,
        /*...float......................................depthBiasClamp.............*/ 0.0,
        /*...float......................................depthBiasSlopeFactor.......*/ 0.0,
        /*...float......................................lineWidth..................*/ 1.0f
    };

    // From Khronos sample: | Our attachment will write to all color channels, but no blending is enabled.  |
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo blendState =
    {
        /*...VkStructureType...............................sType.................*/ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        /*...const.void*...................................pNext.................*/ 0,
        /*...VkPipelineColorBlendStateCreateFlags..........flags.................*/ 0,
        /*...VkBool32......................................logicOpEnable.........*/ VK_FALSE,
        /*...VkLogicOp.....................................logicOp...............*/ (VkLogicOp)0, // This value only used if logicOpEnable is VK_TRUE
        /*...uint32_t......................................attachmentCount.......*/ 1,
        /*...const.VkPipelineColorBlendAttachmentState*....pAttachments..........*/ &colorBlendAttachmentState,
        /*...float.........................................blendConstants[4].....*/ 0
    };

    VkViewport viewport =
    {
        /*...float....x.........*/ 0,
        /*...float....y.........*/ 0,
        /*...float....width.....*/ float(pExtent->width),
        /*...float....height....*/ float(pExtent->height),
        /*...float....minDepth..*/ 0.0f,
        /*...float....maxDepth..*/ 1.0f
    };

    VkRect2D scissor =
    {
        /*...VkOffset2D....offset....*/ { 0, 0 },
        /*...VkExtent2D....extent....*/ {pExtent->width, pExtent->height}
    };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo =
    {
        /*...VkStructureType.......................sType............*/ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        /*...const.void*...........................pNext............*/ 0,
        /*...VkPipelineViewportStateCreateFlags....flags............*/ 0, //reserved for future use as of vk1.3
        /*...uint32_t..............................viewportCount....*/ 1,
        /*...const.VkViewport*.....................pViewports.......*/ &viewport, //@NOTE: Why is this not set in the khronos example? spec says this member is ignored if 
        /*...uint32_t..............................scissorCount.....*/ 1,
        /*...const.VkRect2D*.......................pScissors........*/ &scissor
    };

    // Disabling all depth testing for now
    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo =
    {
        /*...VkStructureType...........................sType..................*/ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        /*...const.void*...............................pNext..................*/ 0,
        /*...VkPipelineDepthStencilStateCreateFlags....flags..................*/ 0,
        /*...VkBool32..................................depthTestEnable........*/ VK_FALSE,
        /*...VkBool32..................................depthWriteEnable.......*/ VK_FALSE,
        /*...VkCompareOp...............................depthCompareOp.........*/ VK_COMPARE_OP_NEVER, // VK_COMPARE_OP_NEVER  == 0
        /*...VkBool32..................................depthBoundsTestEnable..*/ VK_FALSE,
        /*...VkBool32..................................stencilTestEnable......*/ VK_FALSE,
        /*...VkStencilOpState..........................front..................*/ {},
        /*...VkStencilOpState..........................back...................*/ {},
        /*...float.....................................minDepthBounds.........*/ 0.0f,
        /*...float.....................................maxDepthBounds.........*/ 0.0f
    };

    // No Multisampling. 
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo =
    {
        /*...VkStructureType..........................sType.....................*/ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        /*...const.void*..............................pNext.....................*/ 0,
        /*...VkPipelineMultisampleStateCreateFlags....flags.....................*/ 0,
        /*...VkSampleCountFlagBits....................rasterizationSamples......*/ VK_SAMPLE_COUNT_1_BIT,
        /*...VkBool32.................................sampleShadingEnable.......*/ VK_FALSE,
        /*...float....................................minSampleShading..........*/ 0.0,      // Dont think this is used unless sampleShadingEnable is true. 
        /*...const.VkSampleMask*......................pSampleMask...............*/ 0,        // @VK_SPEC: If pSampleMask is NULL, it is treated as if the mask has all bits set to 1.
        /*...VkBool32.................................alphaToCoverageEnable.....*/ false,
        /*...VkBool32.................................alphaToOneEnable..........*/ VK_FALSE  // @VK_SPEC: "If the alphaToOne feature is not enabled, alphaToOneEnable must be VK_FALSE"
    };

    // From the khronos sample: | Specify that these states will be dynamic, i.e. not part of pipeline state object. |
    VkDynamicState dynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo =
    {
        /*...VkStructureType......................sType..................*/ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        /*...const.void*..........................pNext..................*/ 0,
        /*...VkPipelineDynamicStateCreateFlags....flags..................*/ 0,
        /*...uint32_t.............................dynamicStateCount......*/ 2,
        /*...const.VkDynamicState*................pDynamicStates.........*/ dynamicStates
    };

    static const uint32_t vertexShaderIdx   = 0;
    static const uint32_t fragmentShaderIdx = 1;
    static const uint32_t numStages         = 2;

    VkShaderModule pShaderModules[2];

    pShaderModules[fragmentShaderIdx] = CreateShaderModule(logicalDevice, fragShaderPath,true, false);
    pShaderModules[ vertexShaderIdx ] = CreateShaderModule(logicalDevice, vertShaderPath, false, true);


    VkPipelineShaderStageCreateInfo pShaderStageCreateInfos[numStages] =
    {
        {
            /*..VkStructureType.....................sType....................*/ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            /*..const.void*.........................pNext....................*/ 0,
            /*..VkPipelineShaderStageCreateFlags....flags....................*/ 0,
            /*..VkShaderStageFlagBits...............stage....................*/ VK_SHADER_STAGE_VERTEX_BIT,
            /*..VkShaderModule......................module...................*/ pShaderModules[vertexShaderIdx],
            /*..const.char*.........................pName....................*/ "main",
            /*..const.VkSpecializationInfo*.........pSpecializationInfo......*/ 0,
        },
        {
            /*..VkStructureType.....................sType....................*/ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            /*..const.void*.........................pNext....................*/ 0,
            /*..VkPipelineShaderStageCreateFlags....flags....................*/ 0,
            /*..VkShaderStageFlagBits...............stage....................*/ VK_SHADER_STAGE_FRAGMENT_BIT,
            /*..VkShaderModule......................module...................*/ pShaderModules[fragmentShaderIdx],
            /*..const.char*.........................pName....................*/ "main",
            /*..const.VkSpecializationInfo*.........pSpecializationInfo......*/ 0
        }
    };

    assert(pShaderModules[vertexShaderIdx]   != VK_NULL_HANDLE);
    assert(pShaderModules[fragmentShaderIdx] != VK_NULL_HANDLE);
    assert(renderpass                        != VK_NULL_HANDLE);
    assert(pipelineLayout                    != VK_NULL_HANDLE);

    VkGraphicsPipelineCreateInfo gfxPipelineCreateInfo =
    {
        /*...VkStructureType..................................sType...................*/ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        /*...const.void*......................................pNext...................*/ 0,
        /*...VkPipelineCreateFlags............................flags...................*/ 0,
        /*...uint32_t.........................................stageCount..............*/ numStages,
        /*...const.VkPipelineShaderStageCreateInfo*...........pStages.................*/ pShaderStageCreateInfos,
        /*...const.VkPipelineVertexInputStateCreateInfo*......pVertexInputState.......*/ &vertexInputStateCreateInfo,
        /*...const.VkPipelineInputAssemblyStateCreateInfo*....pInputAssemblyState.....*/ &pipelineInputAssemblyStateCreateInfo,
        /*...const.VkPipelineTessellationStateCreateInfo*.....pTessellationState......*/ nullptr,
        /*...const.VkPipelineViewportStateCreateInfo*.........pViewportState..........*/ &viewportStateCreateInfo,
        /*...const.VkPipelineRasterizationStateCreateInfo*....pRasterizationState.....*/ &rasterizationStateCreateInfo,
        /*...const.VkPipelineMultisampleStateCreateInfo*......pMultisampleState.......*/ &multisampleStateCreateInfo,
        /*...const.VkPipelineDepthStencilStateCreateInfo*.....pDepthStencilState......*/ &depthStencilCreateInfo,
        /*...const.VkPipelineColorBlendStateCreateInfo*.......pColorBlendState........*/ &blendState,
        /*...const.VkPipelineDynamicStateCreateInfo*..........pDynamicState...........*/ &dynamicStateCreateInfo,
        /*...VkPipelineLayout.................................layout..................*/ pipelineLayout,
        /*...VkRenderPass.....................................renderPass..............*/ renderpass,
        /*...uint32_t.........................................subpass.................*/ 0,
        /*...VkPipeline.......................................basePipelineHandle......*/ 0,
        /*...int32_t..........................................basePipelineIndex.......*/ 0
    };

    VkPipeline gfxPipeline = VK_NULL_HANDLE;

    // Create the graphics pipeline object
    result = vkCreateGraphicsPipelines(/*..VkDevice.............................device............*/ logicalDevice,
                                       /*..VkPipelineCache......................pipelineCache.....*/ VK_NULL_HANDLE,
                                       /*..uint32_t.............................createInfoCount...*/ 1,
                                       /*..const.VkGraphicsPipelineCreateInfo*..pCreateInfos......*/ &gfxPipelineCreateInfo,
                                       /*..const.VkAllocationCallbacks*.........pAllocator........*/ nullptr,
                                       /*..VkPipeline*..........................pPipelines........*/ &gfxPipeline);
    assert(gfxPipeline != VK_NULL_HANDLE);
    
    vkDestroyShaderModule(logicalDevice, pShaderModules[vertexShaderIdx],   nullptr);
    vkDestroyShaderModule(logicalDevice, pShaderModules[fragmentShaderIdx], nullptr);

    return gfxPipeline;
}


void CreateFrameBuffers(VkDevice                    logicalDevice,
                        VkRenderPass                renderPass,
                        VkExtent2D*                 pExtent,
                        uint32_t                    numSwapchainImages,
                        PerSwapchainImageResources* pPerSwapchainImageResources)
{
    VkResult                result                = VK_INCOMPLETE;
    VkFramebufferCreateInfo framebufferCreateInfo =
    {
        /*...VkStructureType.............sType.............*/ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        /*...const.void*.................pNext.............*/ nullptr, //@VKSPec: "pNext must be NULL or a pointer to a valid instance of VkFramebufferAttachmentsCreateInfo"
        /*...VkFramebufferCreateFlags....flags.............*/ 0,
        /*...VkRenderPass................renderPass........*/ renderPass,
        /*...uint32_t....................attachmentCount...*/ 1,
        /*...const.VkImageView*..........pAttachments......*/ 0, // pAttachments is set from inside the loop below.
        /*...uint32_t....................width.............*/ pExtent->width,
        /*...uint32_t....................height............*/ pExtent->height,
        /*...uint32_t....................layers............*/ 1
    };

    assert(renderPass != VK_NULL_HANDLE);
     
    for (uint32_t viewIdx = 0; viewIdx < numSwapchainImages; viewIdx++)
    {
        framebufferCreateInfo.pAttachments = &pPerSwapchainImageResources[viewIdx].imageView;

        VkFramebuffer* pFramebuffer = &pPerSwapchainImageResources[viewIdx].framebufferHandle;
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

    //printf ("fence[%u](0x%016x) status is %s\n",
    //        swapchainImageIdx,
    //        pSwapchainImageResources->queueSubmitFence,
    //        VkResultToString (fenceStatus));

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





uint64_t ExecuteRenderLoop(VkDevice                     logicalDevice,
                           VkPhysicalDevice             physicalDevice, 
                           VkSwapchainKHR               swapchain,
                           VkQueue                      queue,
                           uint32_t                     gfxQueueIdx,
                           uint32_t                     numPreferredSwapchainFormats,
                           VkFormat*                    pPreferredSwapchainFormats,
                           VkSurfaceKHR                 surface,
                           VkRenderPass                 renderpass,
                           VkPipeline                   pipeline,
                           PerSwapchainImageResources** ppPerSwapchainImageResources,
                           uint32_t*                    pNumSwapchainImages,
                           VkExtent2D*                  pExtent,
                           GeometryBufferSet*           pGeometryBufferSet,
                           uint32_t                     frameIdx)
{
    uint64_t                    time                = 0; ///@TODO: Look into any light-weight profiling measurements that can be taken here and returned from the function.
    VkResult                    result              = VK_INCOMPLETE;
    VkCommandBuffer             renderCommandBuffer = VK_NULL_HANDLE;
    PerSwapchainImageResources* pPerFrameResources  = *ppPerSwapchainImageResources;

    uint32_t imageIdx;
    result = AcuireNextSwapchainImageIdx(queue, logicalDevice, swapchain, &imageIdx, pPerFrameResources);

    // This is expected to be hit when the window has resized, or some other change to the rendering surface that requires reinitialization
    if ((result == VK_SUBOPTIMAL_KHR) || (result == VK_ERROR_OUT_OF_DATE_KHR))
    {
        printf("DONT WANT TO BE HERE!!!!!!\n");
        swapchain = ReinitializeRenderungSurface(logicalDevice,
                                                 physicalDevice,
                                                 gfxQueueIdx,
                                                 swapchain,
                                                 *pExtent,
                                                 numPreferredSwapchainFormats,
                                                 pPreferredSwapchainFormats,
                                                 surface,
                                                 renderpass,
                                                 queue,
                                                 pNumSwapchainImages,
                                                 ppPerSwapchainImageResources);

        result = AcuireNextSwapchainImageIdx(queue, logicalDevice, swapchain, &imageIdx, pPerFrameResources);
    }


    VkClearValue clearValArray[] =
    {
        {0.01f, 0.01f, 0.033f, 1.0f},
        {0.01f, 0.033f, 0.01f, 1.0f},
        {0.033f, 0.01f, 0.01f, 1.0f}
    };
   
    renderCommandBuffer = RecordRenderGeometryBufferCmds (/*...GeometryBufferSet*..........pGeometryBufferSet.............*/ pGeometryBufferSet,
                                                          /*...PerSwapchainImageResources*.pPerSwapchainImageResources....*/ &(pPerFrameResources[imageIdx]),
                                                          /*...VkRenderPass................renderPass.....................*/ renderpass,
                                                          /*...VkPipeline..................pipeline.......................*/ pipeline,
                                                          /*...VkExtent2D*.................pExtent........................*/ pExtent,
                                                          /*...VkClearValue*...............pClearValue....................*/ &(clearValArray[frameIdx % 3]));


    result = SubmitRenderCommandBuffer (/*...VkCommandBuffer.............commandBuffer.....................*/ renderCommandBuffer,
                                        /*...VkQueue.....................queue.............................*/ queue,
                                        /*...PerSwapchainImageResources*.pPerSwapchainImageResources.......*/ &(pPerFrameResources[imageIdx]));

    // present image
    result = PresentImage(swapchain, imageIdx, pPerFrameResources[imageIdx].releaseSwapchainImageSemaphore, queue);
   
    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        printf ("Present image returned %s. May not be able to recover.\n", (result == VK_SUBOPTIMAL_KHR) ? "VK_SUBOPTIMAL_KHR" : "VK_ERROR_OUT_OF_DATE_KHR");
        swapchain = ReinitializeRenderungSurface(logicalDevice,
                                                 physicalDevice,
                                                 gfxQueueIdx,
                                                 swapchain,
                                                 *pExtent,
                                                 numPreferredSwapchainFormats,
                                                 pPreferredSwapchainFormats,
                                                 surface,
                                                 renderpass,
                                                 queue,
                                                 pNumSwapchainImages,
                                                 ppPerSwapchainImageResources);
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
                                            uint32_t                     numPreferredSwapchainFormats,
                                            VkFormat*                    pPreferredSwapchainFormats,
                                            VkSurfaceKHR                 surface,
                                            VkRenderPass                 renderpass,
                                            VkQueue                      queue,
                                            uint32_t*                    pNumSwapchainImages,
                                            PerSwapchainImageResources** ppPerSwapchainImageResources)
{
    assert(logicalDevice != VK_NULL_HANDLE);

    VkResult                 result              = VK_INCOMPLETE;
    VkSurfaceCapabilitiesKHR surfaceCapabilities = {};

    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

    VkSwapchainKHR     newSwapChain              = VK_NULL_HANDLE;

    //If window/surface resolution has changed
    if ((surfaceCapabilities.currentExtent.height != swapchainExtent.height) ||
        (surfaceCapabilities.currentExtent.width  != swapchainExtent.width))
    {
        vkDeviceWaitIdle(logicalDevice);

        //teardown_framebuffers
        vkQueueWaitIdle(queue);

        PerSwapchainImageResources* pPerSwapchainImageResources = *ppPerSwapchainImageResources;
        VkExtent2D                  newSwapChainExtent          = {};
        VkSurfaceFormatKHR          newSwapchainSurfaceFormat   = {};

        InitializeSwapchain (/*.VkPhysicalDevice.............physicalDevice...............*/ physicalDevice,
                             /*.VkDevice.....................logicalDevice................*/ logicalDevice,
                             /*.uint32_t.....................graphicsQueueIndex...........*/ gfxQueueIndex,
                             /*.VkSurfaceKHR.................surface......................*/ surface,
                             /*.uint32_t.....................numPreferredSurfaceFormats...*/ numPreferredSwapchainFormats,
                             /*.VkFormat*....................pPreferredSurfaceFormats.....*/ pPreferredSwapchainFormats,
                             /*.VkExtent2D...................prefferredExtent.............*/ swapchainExtent,
                             /*.VkSwapchainKHR...............oldSwapchain.................*/ swapchain,
                             /*.VkSwapchainKHR*..............pSwapchainOut................*/ &newSwapChain,
                             /*.VkExtent2D*..................pSwapchainExtentChosenOut....*/ &newSwapChainExtent,
                             /*.VkSurfaceFormatKHR*..........pSurfaceFormatOut............*/ &newSwapchainSurfaceFormat,
                             /*.uint32_t*....................pNumSwapchainImages..........*/ pNumSwapchainImages,
                             /*.PerSwapchainImageResources**.ppPerSwapchainImageResources.*/ ppPerSwapchainImageResources);

        assert ((swapchainExtent.height == newSwapChainExtent.height) &&
                (swapchainExtent.width  == newSwapChainExtent.width ));

        CreateFrameBuffers(/*...VkDevice....................logicalDevice....................*/ logicalDevice,
                           /*...VkRenderPass................renderPass.......................*/ renderpass,
                           /*...VkExtent2D*.................pExtent..........................*/ &swapchainExtent,
                           /*...uint32_t....................numSwapchainImages...............*/ *pNumSwapchainImages,
                           /*...PerSwapchainImageResources*.pPerSwapchainImageResources......*/ *ppPerSwapchainImageResources);
    }
    else
    {
        newSwapChain = swapchain; 
    }

    return newSwapChain;
}

VkCommandBuffer RecordRenderGeometryBufferCmds (GeometryBufferSet*          pGeometryBufferSet,
                                                PerSwapchainImageResources* pPerSwapchainImageResources,
                                                VkRenderPass                renderPass,
                                                VkPipeline                  pipeline,
                                                VkExtent2D*                 pExtent,
                                                VkClearValue*               pClearValue)
{
    assert (pGeometryBufferSet != nullptr);
    assert (pExtent            != nullptr);
    assert (pClearValue        != nullptr);

    VkFramebuffer   framebuffer   = pPerSwapchainImageResources->framebufferHandle;
    VkCommandBuffer commandBuffer = pPerSwapchainImageResources->commandBuffer;

    VkCommandBufferBeginInfo cmdBufferBeginInfo =
    {
        /*...VkStructureType..........................sType................*/ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        /*...const.void*..............................pNext................*/ 0,
        /*...VkCommandBufferUsageFlags................flags................*/ VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        /*...const.VkCommandBufferInheritanceInfo*....pInheritanceInfo.....*/ 0
    };

    // Begin command buffer
    vkBeginCommandBuffer (commandBuffer, &cmdBufferBeginInfo);

    VkRect2D renderArea =
    {
        /*...VkOffset2D....offset....*/ {0, 0},
        /*...VkExtent2D....extent....*/ *pExtent
    };

    VkRenderPassBeginInfo renderPassBeginInfo =
    {
        /*...VkStructureType........sType..............*/ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        /*...const.void*............pNext..............*/ 0,
        /*...VkRenderPass...........renderPass.........*/ renderPass,
        /*...VkFramebuffer..........framebuffer........*/ framebuffer,
        /*...VkRect2D...............renderArea.........*/ renderArea,
        /*...uint32_t...............clearValueCount....*/ 1,
        /*...const.VkClearValue*....pClearValues.......*/ pClearValue
    };

    vkCmdBeginRenderPass (commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline (commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

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
    vkCmdBindVertexBuffers (/*..VkCommandBuffer........commandBuffer....*/ commandBuffer,
                            /*..uint32_t...............firstBinding.....*/ 0,
                            /*..uint32_t...............bindingCount.....*/ 1,
                            /*..const.VkBuffer*........pBuffers.........*/ &(pGeometryBufferSet->vertexBufferInfo.bufferHandle),
                            /*..const.VkDeviceSize*....pOffsets.........*/ offsets);

    vkCmdBindIndexBuffer (/*...VkCommandBuffer..........commandBuffer....*/ commandBuffer,
                          /*...VkBuffer.................buffer...........*/ pGeometryBufferSet->indexBufferInfo.bufferHandle,
                          /*...VkDeviceSize.............offset...........*/ 0,
                          /*...VkIndexType..............indexType........*/ VK_INDEX_TYPE_UINT32);

    vkCmdDrawIndexed (/*...VkCommandBuffer.......commandBuffer......*/ commandBuffer,
                      /*...uint32_t..............indexCount.........*/ pGeometryBufferSet->numTriangles * NUM_VERTICES_PER_TRIANGLE,
                      /*...uint32_t..............instanceCount......*/ 1,
                      /*...uint32_t..............firstIndex.........*/ 0,
                      /*...int32_t...............vertexOffset.......*/ 0,
                      /*...uint32_t..............firstInstance......*/ 0);

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
                      