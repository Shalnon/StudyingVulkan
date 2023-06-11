#ifndef VULKAN_ENUM_STRINGS_H
#define VULKAN_ENUM_STRINGS_H

#include "StudyingVulkan.h"


//just the first dozen or so since they have contiguous values.
static const char* VK_RESULT_STRINGS[] =
{
	"VK_SUCCESS",                      //[0] = 0,
	"VK_NOT_READY",                    //[1] = 1,
	"VK_TIMEOUT",                      //[2] = 2,
	"VK_EVENT_SET",                    //[3] = 3,
	"VK_EVENT_RESET",                  //[4] = 4,
	"VK_INCOMPLETE",                   //[5] = 5,
	"VK_ERROR_OUT_OF_HOST_MEMORY",     //[6] = -1,
	"VK_ERROR_OUT_OF_DEVICE_MEMORY",   //[7] = -2,
	"VK_ERROR_INITIALIZATION_FAILED",  //[8] = -3,
	"VK_ERROR_DEVICE_LOST",            //[9] = -4,
	"VK_ERROR_MEMORY_MAP_FAILED",      //[10] = -5,
	"VK_ERROR_LAYER_NOT_PRESENT",      //[12] = -6,
	"VK_ERROR_EXTENSION_NOT_PRESENT",  //[13] = -7,
	"VK_ERROR_FEATURE_NOT_PRESENT",    //[14] = -8,
	"VK_ERROR_INCOMPATIBLE_DRIVER",    //[15] = -9,
	"VK_ERROR_TOO_MANY_OBJECTS",       //[16] = -10,
	"VK_ERROR_FORMAT_NOT_SUPPORTED",   //[17] = -11,
	"VK_ERROR_FRAGMENTED_POOL",        //[18] = -12,
	"VK_ERROR_UNKNOWN"                 //[19] = -13,

    //  (result * -1) + 5

    //VK_ERROR_DEVICE_LOST = -4

    // (-4 * -1) + 5 // 4 + 5
/*
    "VK_ERROR_OUT_OF_POOL_MEMORY",                           // = -1000069000, // FFFFFFFFC4642878    1,000,069,000
    "VK_ERROR_INVALID_EXTERNAL_HANDLE",                      // = -1000072003,// FFFF FFFF C464 1CBD
    "VK_ERROR_FRAGMENTATION",                                // = -1000161000, // 1,000,072,003
    "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS",               // = -1000257000, // 1,000,257,000
    "VK_PIPELINE_COMPILE_REQUIRED",                          // = 1000297000,
    "VK_ERROR_SURFACE_LOST_KHR",                             // = -1000000000,
    "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR",                     // = -1000000001,
    "VK_SUBOPTIMAL_KHR",                                     // = 1000001003,
    "VK_ERROR_OUT_OF_DATE_KHR",                              // = -1000001004,
    "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR",                     // = -1000003001,
    "VK_ERROR_VALIDATION_FAILED_EXT",                        // = -1000011001,
    "VK_ERROR_INVALID_SHADER_NV",                            // = -1000012000,
    "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT", // = -1000158000,
    "VK_ERROR_NOT_PERMITTED_KHR",                            // = -1000174001,
    "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT",          // = -1000255000,
    "VK_THREAD_IDLE_KHR",                                    // = 1000268000,
    "VK_THREAD_DONE_KHR",                                    // = 1000268001,
    "VK_OPERATION_DEFERRED_KHR",                             // = 1000268002,
    "VK_OPERATION_NOT_DEFERRED_KHR",                         // = 1000268003,
    "VK_ERROR_OUT_OF_POOL_MEMORY_KHR",                       // = "VK_ERROR_OUT_OF_POOL_MEMORY",
    "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR",                  // = "VK_ERROR_INVALID_EXTERNAL_HANDLE",
    "VK_ERROR_FRAGMENTATION_EXT",                            // = "VK_ERROR_FRAGMENTATION",
    "VK_ERROR_NOT_PERMITTED_EXT",                            // = "VK_ERROR_NOT_PERMITTED_KHR",
    "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT",                   // = "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS",
    "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR",           // = "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS",
    "VK_PIPELINE_COMPILE_REQUIRED_EXT",                      // = "VK_PIPELINE_COMPILE_REQUIRED",
    "VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT",                // = "VK_PIPELINE_COMPILE_REQUIRED",
    "VK_RESULT_MAX_ENUM"                                     // = 0x7FFFFFFF
*/
};

inline const char* VkResultToString(VkResult result)
{
    const char* pResultStr = nullptr;

    if (result > -1)
    {
        pResultStr = VK_RESULT_STRINGS[result];
    }
    else if ((result > -14) && (result < 64)) // VK_ERROR_UNKNOWN = -13
    {
        assert((result * -1) + 5 >= 0);
        pResultStr = VK_RESULT_STRINGS[(result * -1) + 5];
    }
    else
    {
        switch (result)
        {
            case VK_ERROR_OUT_OF_POOL_MEMORY:
                pResultStr = "VK_ERROR_OUT_OF_POOL_MEMORY";
                break;
            case VK_ERROR_INVALID_EXTERNAL_HANDLE:
                pResultStr = "VK_ERROR_INVALID_EXTERNAL_HANDLE";
                break;
            case VK_ERROR_FRAGMENTATION:
                pResultStr = "VK_ERROR_FRAGMENTATION";
                break;
            case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
                pResultStr = "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS";
                break;
            case VK_PIPELINE_COMPILE_REQUIRED:
                pResultStr = "VK_PIPELINE_COMPILE_REQUIRED";
                break;
            case VK_ERROR_SURFACE_LOST_KHR:
                pResultStr = "VK_ERROR_SURFACE_LOST_KHR";
                break;
            case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
                pResultStr = "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
                break;
            case VK_SUBOPTIMAL_KHR:
                pResultStr = "VK_SUBOPTIMAL_KHR";
                break;
            case VK_ERROR_OUT_OF_DATE_KHR:
                pResultStr = "VK_ERROR_OUT_OF_DATE_KHR";
                break;
            case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
                pResultStr = "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
                break;
            case VK_ERROR_VALIDATION_FAILED_EXT:
                pResultStr = "VK_ERROR_VALIDATION_FAILED_EXT";
                break;
            case VK_ERROR_INVALID_SHADER_NV:
                pResultStr = "VK_ERROR_INVALID_SHADER_NV";
                break;
            case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
                pResultStr = "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
                break;
            case VK_ERROR_NOT_PERMITTED_KHR:
                pResultStr = "VK_ERROR_NOT_PERMITTED_KHR";
                break;
            case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
                pResultStr = "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
                break;
            case VK_THREAD_IDLE_KHR:
                pResultStr = "VK_THREAD_IDLE_KHR";
                break;
            case VK_THREAD_DONE_KHR:
                pResultStr = "VK_THREAD_DONE_KHR";
                break;
            case VK_OPERATION_DEFERRED_KHR:
                pResultStr = "VK_OPERATION_DEFERRED_KHR";
                break;
            case VK_OPERATION_NOT_DEFERRED_KHR:
                pResultStr = "VK_OPERATION_NOT_DEFERRED_KHR";
                break;
            case VK_RESULT_MAX_ENUM:
                pResultStr = "VK_RESULT_MAX_ENUM";
                break;
            default:
                printf("invalid result type!!!\n");
                break;
        }
    }

    return pResultStr;


}

#endif // VULKAN_ENUM_STRINGS_H