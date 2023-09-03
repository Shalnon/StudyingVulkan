#ifndef VULKAN_COMPUTE_H
#define VULKAN_COMPUTE_H

#include "StudyingVulkan.h"
#include "Vulkan_Utils.h"
#include "AppWindowingSystem.h"
#include "resource.h"
#include "Vulkan_Synchronization.h"
#include <limits>

namespace ComputeParameters
{
    static const size_t binSize               = 3; // can range from  2 - 128
    static const size_t inputArraySize        = 128;//always a multiple of 4
    static const size_t numBytesPerNum        = 1;

    static const uint8_t numRangeMin = 0;
    static const uint8_t numRangeMax = 255;

    static const uint32_t numDescriptorSets      = 1;
    static const uint32_t numDescriptorTypesUsed = 2;
    static const uint32_t numUBOs                = 1;
    static const uint32_t numSSBOs               = 1;
    static const uint32_t numDescriptors         = numUBOs + numSSBOs;

    struct UboDataLayout
    {
        //uint32_t binSize;
        //uint32_t numPackedChars;
        uint32_t  numArray[inputArraySize];
    };
}

void RunComputeExample (const char*      pComputeShaderPath,
                        VkPhysicalDevice physicalDevice,
                        VkDevice         logicalDevice,
                        uint32_t         queueIndex,
                        VkQueue          queueHandle);

#endif