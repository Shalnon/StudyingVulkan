# Copyright 2023 Sean Halnon
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
# 
#  http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissionsand
#  limitations under the License.

cmake_minimum_required (VERSION 3.11)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project ("StudyingVulkan")

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
SET(VERSION_PATCH 0)

option(RUNTIME_DEBUG_PRINTS_ENABLE OFF)

set(SHADERS_ROOT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders")
set(ASSETS_DIRECTORY       "${CMAKE_CURRENT_SOURCE_DIR}/Assets")


if(RUNTIME_DEBUG_PRINTS_ENABLE)
     set(DEBUG_PRINTS_ENABLE 1)
else()
    set(DEBUG_PRINTS_ENABLE 0)
endif()


configure_file(Source/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/Source/config.h @ONLY)

# Add source to this project's executable.
add_executable (StudyingVulkan "Source/AppWindowingSystem.cpp"
                               "Source/AppWindowingSystem.h"
                               "Source/Shaders/Khronos/frag.spv"
                               "Source/Shaders/Khronos/triangle.frag"
                               "Source/Shaders/Khronos/triangle.vert"
                               "Source/Shaders/Khronos/vert.spv"
                               "Source/StudyingVulkan.cpp"
                               "Source/StudyingVulkan.h"
                               "Source/Vulkan_Synchronization.cpp"
                               "Source/Vulkan_Synchronization.h"
                               "Source/Vulkan_Utils.cpp"
                               "Source/Vulkan_Utils.h"
                               "Source/Vulkan_enum_strings.h"
                               "Source/WinResources.rc"
                               "Source/resource.h"
                               "Source/Vulkan_Buffer_tools.h"
                               "Source/Vulkan_Buffer_Tools.cpp"
                               "Source/Asset_Tools.h"
                               "Source/Asset_Tools.cpp"
                               "Source/Vulkan_Descriptor_Tools.cpp"
                               "Source/Vulkan_Descriptor_Tools.h" 
                               "Source/Logging.h")

set_property(TARGET StudyingVulkan PROPERTY CXX_STANDARD 17)

# Variables for local use
set (LIB_GLM_NAME    "glm"   )
set (LIB_ASSIMP_NAME "assimp")
set (LIB_VOLK_NAME   "volk"  )
set (EXTERNAL_LIBRARIES_DIRECTORY "${CMAKE_HOME_DIRECTORY}/3rdPartyLibraries")

include(FetchContent)
FetchContent_Declare(glm
                     GIT_REPOSITORY https://github.com/g-truc/glm.git
                     GIT_TAG efec5db081e3aad807d0731e172ac597f6a39447
                     SOURCE_DIR "${EXTERNAL_LIBRARIES_DIRECTORY}/${LIB_GLM_NAME}")


FetchContent_Declare(assimp
                     GIT_REPOSITORY https://github.com/assimp/assimp.git
                     GIT_TAG 45946ec01bc2f4678ba62c9151f3f916842e1fc5
                     SOURCE_DIR "${EXTERNAL_LIBRARIES_DIRECTORY}/${LIB_ASSIMP_NAME}"
                     BINARY_DIR "${EXTERNAL_LIBRARIES_DIRECTORY}/${LIB_ASSIMP_NAME}")

FetchContent_Declare(volk
                     GIT_REPOSITORY https://github.com/zeux/volk.git
                     GIT_TAG 5e9877395a6e20fb3f054e4e2a1a89f8d568a952
                     SOURCE_DIR "${EXTERNAL_LIBRARIES_DIRECTORY}/${LIB_VOLK_NAME}")

option( ASSIMP_INSTALL           ""  OFF )
option( ASSIMP_BUILD_TESTS       ""  OFF )
option( BUILD_SHARED_LIBS        ""  OFF ) #: Generation of shared libs (dll for windows, .so for Linux). Set this to OFF to get a static lib.
option( ASSIMP_BUILD_ASSIMP_VIEW ""  OFF )
option( ASSIMP_NO_EXPORT         ""  ON  )

target_include_directories(StudyingVulkan PUBLIC "${EXTERNAL_LIBRARIES_DIRECTORY}/${LIB_GLM_NAME}" )
target_include_directories(StudyingVulkan PUBLIC "${EXTERNAL_LIBRARIES_DIRECTORY}/${LIB_ASSIMP_NAME}" )
target_include_directories(StudyingVulkan PUBLIC "${EXTERNAL_LIBRARIES_DIRECTORY}/${LIB_VOLK_NAME}" )

add_dependencies(StudyingVulkan assimp)
add_dependencies(StudyingVulkan volk)

target_link_libraries(StudyingVulkan assimp)
target_link_libraries(StudyingVulkan volk)

FetchContent_MakeAvailable( assimp volk glm)

# Include sub-projects.
add_subdirectory ("${EXTERNAL_LIBRARIES_DIRECTORY}")
