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

#ifndef STUDYING_VULKAN_H
#define STUDYING_VULKAN_H

#include <iostream>
#include <assert.h> 
#include <stdio.h>
#include <stdlib.h>
//#define WIN32_LEAN_AND_MEAN   
#include <windows.h>
#include <filesystem>
#pragma once
#include <fstream>
#include <cstdlib>


#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "config.h"


//@TODO: Define a macro to do debug prints instead of manually placing #ifdef DEBUG around every print
#if DEBUG_PRINTS_ENABLE
#define DEBUG
#endif

//#define DEBUG
#define VK_USE_PLATFORM_WIN32_KHR
#define USE_VOLK

#if defined(USE_VOLK)
    //#define   VOLK_IMPLEMENTATION //@note: Defined in vulkan_utils.cpp
    #include "volk.h"
#else
    #include "vulkan/vulkan.h"
	#define VK_EXPORTED_FUNCTION(func) extern PFN_##func func;
#endif

// used to map data to the vertex attributes

#define WINDOW_WIDTH  1920
#define WINDOW_HEIGHT 1080

#define RADIANS_PER_DEGREE 0.0174532925f

namespace SceneVulkanParameters
{
    static const uint32_t numDescriptorTypesUsedInScene        = 3;
    static const uint32_t numUboDescriptorsInScene             = 1;
    static const uint32_t numSsboDescriptorsInScene            = 1;
    static const uint32_t numInputAttachmentDescriptorsInScene = 4;
    static const uint32_t verticiesPerPrimitive                = 3; // Triangles

    static const uint32_t totalNumDescriporSets = 2;

    static const uint32_t numCoordsPerVertexPosition = 3;
    static const uint32_t numCoordsPerVertexNormal   = 3;
    static const uint32_t numBytesPerVertexPosition  = numCoordsPerVertexPosition * sizeof(float);
    static const uint32_t numBytesPerVertexNormal    = numCoordsPerVertexPosition * sizeof (float);
    static const uint32_t numIndexBytesPerPrimitive  = verticiesPerPrimitive * sizeof (uint32_t);

    //RGBA because there needs to be atleast a 16 byte stride between elements
    static const uint32_t numChannelsPerColor     = 4;
    static const uint32_t numBytesPerColorChannel = sizeof (float);
    static const uint32_t numBytesPerColor        = numChannelsPerColor * numBytesPerColorChannel;

    static const uint32_t numRequiredExtensions   = 1;
    static const char*    requiredDeviceExtensionss[numRequiredExtensions] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    //@NOTE: Ndc bounds on the z axis are [0,1] for vulkan, whereas for opengl it is [-1,1]
    // Parameters used to setup projection matrix 
    static const float horizontal_fov = 70.0f;
    static const float zNear          = 1.0f;
    static const float zFar           = 50.0;

    namespace window
    {
        static const uint32_t width  = WINDOW_WIDTH;
        static const uint32_t height = WINDOW_HEIGHT;
    };
    namespace RenderPassParameters
    {
        static const uint32_t numColorAttachments = 4; // diffuse, normal, position, present
        static const uint32_t numDepthAttachments = 1; // used as depth attachment in subpass 0, than as an input attachment in subpass 1
        static const uint32_t numSubpasses        = 2;
        static const uint32_t totalNumAttachments = numColorAttachments + numDepthAttachments;

        // Renderpass Color attachments indeces
        static const uint32_t swapchainColorAttachmentIndex = 0;
        static const uint32_t diffuseColorAttachmentIndex   = 1;
        static const uint32_t surfaceNormalAttachmentIndex  = 2;
        static const uint32_t positionAttachmentIndex       = 3;
        static const uint32_t depthStencilAttachmentIndex   = 4;
    }
    namespace Subpass0
    {
        //@note: Subpass 0 doesnt write to the swapchain color attachment.
        static const uint32_t numColorAttachments = 3; //diffuse color + normal + position gbuffer images
        static const uint32_t numDepthAttachments = 1;
        static const uint32_t numInputAttachments = 0;

        static const uint32_t depthAttachmentIndex = RenderPassParameters::depthStencilAttachmentIndex;

        static const uint32_t numDescriptorSetsUsed = 1;

        // Descriptor set layout binding information
        static const uint32_t numDescriptorSetLayoutBindings = 2;
        static const uint32_t uniformBufferBinding           = 0; // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
        static const uint32_t storageBufferBinding           = 1; // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER

        //Vertex attrib information for pipeline
        static const uint32_t numInputBindings    = 1; // Number of vertex attribute buffers we need a binding for
        static const uint32_t numVertexAttributes = 2; // Number of vertex input attributes the shader will recieve

        static const uint32_t vertexLocationAttributePosition = 0;
        static const uint32_t vertexNormalAttributePosition   = 1;

        static const char* pFragShaderPath = "/deferredRendererShaders/subpass0/frag.spv";
        static const char* pVertShaderPath = "/deferredRendererShaders/subpass0/vert.spv";
    }
    namespace Subpass1
    {
        //@note: Subpass 1 DOES write to the swapchain color attachment.
        static const uint32_t numColorAttachments = 1; // Present image
        static const uint32_t numDepthAttachments = 0; // depth is disabled for subpass 1
        static const uint32_t numInputAttachments = 3; // diffuse color + normal + position + subpass0 depth

        static const uint32_t pColorAttachmentIndeces[numColorAttachments] =
        {
            RenderPassParameters::swapchainColorAttachmentIndex
        };

        static const uint32_t numDescriptorSetLayoutBindings     = 4; // 3  input attachments and a ubo

        // Descriptor set layout bindings
        static const uint32_t uniformBufferBinding               = 0;
        static const uint32_t diffuseColorInputAttachmentBinding = 1;
        static const uint32_t normalVectorInputAttachmentBinding = 2;
        static const uint32_t positionInputAttachmentBinding     = 3;

        //Vertex attrib information
        static const uint32_t numInputBindings              = 0; // Number of vertex attribute buffers we need a binding for
        static const uint32_t numVertexAttributes           = 0; // Number of vertex input attributes the shader will recieve

        static const uint32_t numDescriptorSetsUsed = 1;

        static const char* pFragShaderPath = "/deferredRendererShaders/subpass1/frag.spv";
        static const char* pVertShaderPath = "/deferredRendererShaders/subpass1/vert.spv";
    };

    static const uint32_t minSemaphoreListSize    = 64;
    static const uint32_t semaphoreListGrowthRate = 64;


    static const VkFormat normalVectorGbufferImageFormat   = VK_FORMAT_R16G16B16A16_SNORM;
    static const VkFormat positionVectorGbufferImageFormat = VK_FORMAT_R32G32B32A32_SFLOAT;//VK_FORMAT_R16G16B16A16_SFLOAT;
    static const VkFormat diffuseColorGbufferImageFormat   = VK_FORMAT_B8G8R8A8_UNORM;

    static const uint32_t numPreferredSwapchainFormats                                   = 2;
    static const uint32_t numPreferredDepthFormats                                       = 2;
    static const VkFormat preferredSwapchainImagesFormats[numPreferredSwapchainFormats]  = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SRGB };
    static const VkFormat preferredDeptAttachmentFormats[]                               = { VK_FORMAT_D32_SFLOAT };
}

// Defining a struct here that matches the UBO data layout described in the shader.
struct UniformBufferData
{
    // Scene Transform
    glm::mat4 sceneTransform;
    glm::vec4 sceneScale;

    glm::mat4 projecrtionMatrix;
    glm::mat4 normalRotation;

    // Scene ambient color
    glm::vec4  ambient_color;

    // Light location
    glm::vec4 lightLocation;
    glm::vec4 lightIntensities;
};

struct VertexAttributeData
{
    float position[3];
    float normal[3];
};

struct AttributeInfo
{
    uint32_t bufferBindingIdx; // VkVertexInputAttributeDescription::binding
    uint32_t attributeIdx;     // VkVertexInputAttributeDescription::location
    uint32_t stride;           // Stride of the attribute. (not the entire vertex)
    uint32_t offset;           // Attribute offset - this will be non-0 when using interleaved attributes. (as in position and normal for a vertex being next to each other in the buffer)
    VkFormat format;           // The format of the data : determines which data goes to which xyzw component
};

// All attributes will come from the same buffer for now
static AttributeInfo s_VertexShaderAttributes[] =
{
    { // POSITION ATTRIBUTE
        /*...uint32_t...bufferBindingIdx..*/ 0,
        /*...uint32_t...attributeIdx......*/ SceneVulkanParameters::Subpass0::vertexLocationAttributePosition, //0
        /*...uint32_t...stride............*/ sizeof (VertexAttributeData::position),
        /*...uint32_t...offset............*/ offsetof (VertexAttributeData, position),
        /*...VkFormat...format............*/ VK_FORMAT_R32G32B32_SFLOAT
    },
    { // VERTEX NORMAL ATTRIBUTE
        /*...uint32_t...bufferBindingIdx..*/ 0,
        /*...uint32_t...attributeIdx......*/ SceneVulkanParameters::Subpass0::vertexNormalAttributePosition, //1
        /*...uint32_t...stride............*/ sizeof (VertexAttributeData::normal),
        /*...uint32_t...offset............*/ offsetof (VertexAttributeData, normal),
        /*...VkFormat...format............*/ VK_FORMAT_R32G32B32_SFLOAT
    }
};

// Ceiling aligns a number
#define ALIGN(num, alignment) (num + (alignment - (num % alignment)))

#endif // STUDYING_VULKAN_H