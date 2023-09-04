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
#ifndef VULKAN_COMPUTE_H
#define VULKAN_COMPUTE_H

#include "BucketSearchVisualization.h"
#include "Vulkan_Utils.h"
#include "AppWindowingSystem.h"
#include "resource.h"
#include "Vulkan_Synchronization.h"
#include <limits>
#include "..\3rdPartyLibraries\glm\glm\glm.hpp"

namespace ComputeParameters
{
    extern size_t binSize; // can range from  2 - 128

    static const size_t defaultBinSize = 4;
    static const size_t minBinSize     = 1;
    static const size_t maxBinSize     = 32;


    static const size_t inputArraySize = 128;//always a multiple of 4
    static const size_t numBytesPerNum = 1;

    static const size_t maxInputArraySize = 128;
    static const size_t minInputArraySize = 64;

    static const uint8_t numRangeMin = 0;
    static const uint8_t numRangeMax = 255;

    static const uint32_t numDescriptorSets      = 1;
    static const uint32_t numDescriptorTypesUsed = 2;
    static const uint32_t numUBOs                = 1;
    static const uint32_t numSSBOs               = 1;
    static const uint32_t numDescriptors         = numUBOs + numSSBOs;

    struct UboDataLayout
    {
        uint32_t binSize;
        uint32_t numPackedChars;
        uint8_t  numArray[inputArraySize];
    };
}

void RunComputeExample (const char*            pComputeShaderPath,
                        VkPhysicalDevice       physicalDevice,
                        VkDevice               logicalDevice,
                        uint32_t               queueIndex,
                        VkQueue                queueHandle,
                        std::vector<uint32_t>& histogram_out);


class Bar
{
public:
    Bar (float height, float width, glm::vec3 position, float floorLevel)
    {
        static const glm::vec3 s_PlaneVertices[6]
        {

            glm::vec3 (0.0, 1.0, 0.5),
            glm::vec3 (1.0, 0.0, 0.5),
            glm::vec3 (0.0, 0.0, 0.5),

            glm::vec3 (0.0, 1.0, 0.5),
            glm::vec3 (1.0, 1.0, 0.5),
            glm::vec3 (1.0, 0.0, 0.5),
        };

        float maxY = floorLevel + height + position.y;
        float minY = floorLevel + position.y;
        float maxX = width + position.x;
        float minX = position.x;

        verts[0] = glm::vec3 (maxX, -minY, 0.5);
        verts[1] = glm::vec3 (minX, -maxY, 0.5);
        verts[2] = glm::vec3 (minX, -minY, 0.5);
        verts[3] = glm::vec3 (maxX, -minY, 0.5);
        verts[4] = glm::vec3 (minX, -maxY, 0.5);
        verts[5] = glm::vec3 (maxX, -maxY, 0.5);

    }
    glm::vec3 verts[6];
};

std::vector<Bar> CreateBarGraphMesh (uint32_t* pBarHeights, uint32_t numBars);

void ExecuteBuffer2BufferCopy (VkPhysicalDevice          physicalDevice,
                               VkDevice                  logicalDevice,
                               VkQueue                   queue,
                               uint32_t                  graphicsQueueIndex,
                               VkDeviceSize              copySize,
                               vulkanAllocatedBufferInfo srcBufferInfo,
                               vulkanAllocatedBufferInfo dstBufferInfo);

#endif