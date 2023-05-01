# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.11)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project ("StudyingVulkan")


# Add source to this project's executable.
add_executable (StudyingVulkan "Source/StudyingVulkan.cpp"
                               "Source/StudyingVulkan.h"
                               "Source/AppWindowingSystem.cpp"
                               "Source/AppWindowingSystem.h"
                               "Source/Asset_Tools.cpp"
                               "Source/Asset_Tools.h"
                               "Source/Vulkan_Buffer_Tools.h"
                               "Source/Vulkan_Buffer_tools.cpp"
                               "Source/Vulkan_Synchronization.cpp"
                               "Source/Vulkan_Synchronization.h"
                               "Source/Vulkan_Utils.cpp"
                               "Source/Vulkan_Utils.h"
                               "Source/Vulkan_enum_strings.h"
                               "Source/WinResources.aps"
                               "Source/WinResources.rc"
                               "Source/resource.h"
                               "Source/Shaders/Khronos/frag.spv"
                               "Source/Shaders/Khronos/triangle.frag"
                               "Source/Shaders/Khronos/triangle.vert"
                               "Source/Shaders/Khronos/vert.spv")

set_property(TARGET StudyingVulkan PROPERTY CXX_STANDARD 17)

# Variables for local use
set (LIB_ASSIMP_NAME "assimp")
set (LIB_VOLK_NAME    "volk" )
set (EXTERNAL_LIBRARIES_DIRECTORY "${CMAKE_HOME_DIRECTORY}/3rdPartyLibraries")

include(FetchContent)
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
option( BUILD_SHARED_LIBS        ""  OFF ) #: Generation of shared libs (dll for windows, so for Linux). Set this to OFF to get a static lib.
option( ASSIMP_BUILD_ASSIMP_VIEW ""  OFF )
option( ASSIMP_NO_EXPORT         ""  ON  )

target_include_directories(StudyingVulkan PUBLIC "${EXTERNAL_LIBRARIES_DIRECTORY}/${LIB_ASSIMP_NAME}" )
target_include_directories(StudyingVulkan PUBLIC "${EXTERNAL_LIBRARIES_DIRECTORY}/${LIB_VOLK_NAME}" )

add_dependencies(StudyingVulkan assimp)
add_dependencies(StudyingVulkan volk)

target_link_libraries(StudyingVulkan assimp)
target_link_libraries(StudyingVulkan volk)

FetchContent_MakeAvailable( assimp 
                            volk )

# Include sub-projects.
add_subdirectory ("${EXTERNAL_LIBRARIES_DIRECTORY}")
