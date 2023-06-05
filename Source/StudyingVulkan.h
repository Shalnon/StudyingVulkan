#ifndef STUDYINGVULKAN_H
#define STUDYINGVULKAN_H

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

#define DEBUG

#define VK_USE_PLATFORM_WIN32_KHR
//#define  VK_NO_PROTOTYPES

//@TODO: Clean up all these defines 

#define USE_VOLK
//Max must be a multiple of 64
#define MAX_SEMAPHORE_LIST_SIZE 512
#define MIN_SEMAPHORE_LIST_SIZE 64
#define SEMAPHORE_LIST_GROWTH_RATE 64
#define COORDS_PER_POSITION 3
#define COORDS_PER_NORMAL 3
#define NUM_VERTICES_PER_TRIANGLE 3
#define NUM_BYTES_PER_VERTEX_POSITION sizeof (float) * COORDS_PER_POSITION
#define NUM_BYTES_PER_NORMAL_VECTOR sizeof (float) * COORDS_PER_POSITION
#define NUM_INDEX_BYTES_PER_TRIANGLE sizeof (uint32_t) * NUM_VERTICES_PER_TRIANGLE

//RGBA because there needs to be a 16 byte stride between elements
#define NUM_CHANNELS_PER_COLOR 4
#define NUM_BYTES_PER_COLOR_CHANNEL sizeof(float)
#define NUM_BYTES_PER_COLOR_VALUE    NUM_CHANNELS_PER_COLOR * NUM_BYTES_PER_COLOR_CHANNEL

#define DIFFUSE_COLOR_ATTACHMENT_IDX  0
#define SURFACE_NORMAL_ATTACHMENT_IDX 1
#define PRESENT_COLOR_ATTACHMENT_IDX  2
#define DEPTH_STENCIL_ATTACHMENT_IDX  3

#define SUBPASS_0_UNIFORM_BUFFER_DESCRIPTOR_BINDING 0
#define SUBPASS_0_STORAGE_BUFFER_DESCRIPTOR_BINDING 1

// Subpass 0 input attachment binding numbers
#define SUBPASS_1_DIFFUSE_COLOR_INPUT_ATTACHMENT_DESCRIPTOR_BINDING 0
#define SUBPASS_1_SURFACE_NORMAL_INPUT_ATTACHMENT_DESCRIPTOR_BINDING 1
#define SUBPASS_1_DEPTH_STENCIL_INPUT_ATTACHMENT_DESCRIPTOR_BINDING 2
#define SUBPASS_1_UNIFORM_BUFFER_DESCRIPTOR_BINDING 3


#define NUM_COLOR_ATTACHMENTS 3
#define NUM_DEPTH_STENCIL_ATTACHMENTS 1
#define NUM_ATTACHMENTS NUM_COLOR_ATTACHMENTS + NUM_DEPTH_STENCIL_ATTACHMENTS

#define NUM_INPUT_ATTACHMENTS 3

#define REQUIRED_SSBO_MEMORY_ALIGNMENT NUM_BYTES_PER_COLOR_VALUE
#define REQUIRED_UBO_MEMORY_ALIGNMENT  sizeof(float) * 16

#define NUM_SUBPASSES 2
#define NUM_DESCRIPTOR_SETS NUM_SUBPASSES

#if defined(USE_VOLK)
    //#define   VOLK_IMPLEMENTATION
    #include "volk.h"
#else
    #include "vulkan/vulkan.h"
	#define VK_EXPORTED_FUNCTION(func) extern PFN_##func func;
#endif




// Ceiling aligns a number
#define ALIGN(num, alignment) (num + (alignment - (num % alignment)))

#define WINDOW_WIDTH  1280
#define WINDOW_HEIGHT 720

//C:\Users\shaln\source\repos\StudyingVulkan\StudyingVulkan\Shaders\Khronos
#define FRAG_PATH  "C:\\git\\CmakeExperiments\\StudyingVulkan\\StudyingVulkan\\Source\\Shaders\\Khronos\\withVertexBuffer\\frag.spv"
#define VERT_PATH  "C:\\git\\CmakeExperiments\\StudyingVulkan\\StudyingVulkan\\Source\\Shaders\\Khronos\\withVertexBuffer\\vert.spv"

#define NUM_PREFERRED_SURFACE_FORMATS 2
#define PREFERRED_SURRFACE_FORMATS  { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SRGB }
#define NUM_PREFFERRED_DEPTH_FORMATS 1
#define PREFERRED_DEPTH_FORMATS { VK_FORMAT_D32_SFLOAT }


extern uint32_t g_FragShader[];
extern uint32_t g_VertShader[];

#endif