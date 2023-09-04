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
#ifndef STUDYINGVULKAN_H
#define STUDYINGVULKAN_H

#include <iostream>
#include <assert.h> 
#include <stdio.h>
#include <stdlib.h>
//#define WIN32_LEAN_AND_MEAN   
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <cstdlib>

#define VK_USE_PLATFORM_WIN32_KHR
//#define  VK_NO_PROTOTYPES

#define USE_VOLK
//Max must be a multiple of 64
#define MAX_SEMAPHORE_LIST_SIZE 512
#define MIN_SEMAPHORE_LIST_SIZE 64
#define SEMAPHORE_LIST_GROWTH_RATE 64



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

#define FRAG_PATH "frag.spv"
#define VERT_PATH "vert.spv"
#define COMP_PATH "calc_histogram.spv"

#define NUM_PREFERRED_SURFACE_FORMATS 2
#define PREFERRED_SURRFACE_FORMATS  { VK_FORMAT_R8G8B8A8_SRGB,  VK_FORMAT_B8G8R8A8_SRGB }
#endif