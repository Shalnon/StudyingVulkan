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

#define VK_USE_PLATFORM_WIN32_KHR
//#define  VK_NO_PROTOTYPES

#define USE_VOLK
//Max must be a multiple of 64
#define MAX_SEMAPHORE_LIST_SIZE 512
#define MIN_SEMAPHORE_LIST_SIZE 64
#define SEMAPHORE_LIST_GROWTH_RATE 64
#define COORDS_PER_POSITION 3
#define NUM_VERTICES_PER_TRIANGLE 3
#define NUM_BYTES_PER_VERTEX_POSITION sizeof (float) * COORDS_PER_POSITION
#define NUM_INDEX_BYTES_PER_TRIANGLE sizeof (uint32_t) * NUM_VERTICES_PER_TRIANGLE

//RGB
#define NUM_COLOR_CHANNELS_PER_VERTEX 3
#define NUM_BYTES_PER_COLOR_CHANNEL sizeof(float)
#define NUM_COLOR_BYTES_PER_VERTEX NUM_COLOR_CHANNELS_PER_VERTEX * NUM_BYTES_PER_COLOR_CHANNEL


//Define our model matrix format. Doing it this way because im not actually sure i need a full 4x4 matrix and this makes it easier to experiment
#define NUM_MODEL_MATRIX_COLUMNS 4
#define NUM_MODEL_MATRIX_ROWS    4 
#define NUM_FLOATS_PER_TRASNFORM_MATRIX NUM_MODEL_MATRIX_COLUMNS * NUM_MODEL_MATRIX_ROWS
#define NUM_BYTES_PER_MODEL_MATRIX   NUM_MODEL_MATRIX_COLUMNS * NUM_MODEL_MATRIX_ROWS * sizeof(float)



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
#define PREFERRED_SURRFACE_FORMATS  { VK_FORMAT_R8G8B8A8_SRGB,  VK_FORMAT_B8G8R8A8_SRGB }
#define NUM_PREFFERRED_DEPTH_FORMATS 1
#define PREFERRED_DEPTH_FORMATS { VK_FORMAT_D32_SFLOAT }


extern uint32_t g_FragShader[];
extern uint32_t g_VertShader[];

#endif