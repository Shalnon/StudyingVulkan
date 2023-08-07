# Vulkan Study - Sean Halnon

Code and Art by Sean Halnon.

Examples are organized as separate branches.

## Overview:
This Repo contains a set of Vulkan API examples illustrating important fundumental usage scenarios.
Other than simple triangle, each branch implements a model viewer using a different rendering strategy.

[ForwardRenderer - noMSAA](https://github.com/Shalnon/StudyingVulkan/tree/ForwardRenderer_noMSAA "Forward Renderer - noMSAA")

[Deffered Renderer - 4xMSAA](https://github.com/Shalnon/StudyingVulkan/tree/DeferredRendering_4xMSAA "Deffered Renderer - 4xMSAA")

[Deffered Renderer - noMSAA](https://github.com/Shalnon/StudyingVulkan/tree/Deffered_Renderer_noMSAA "Deffered Renderer - noMSAA")

[Simple Triangle](https://github.com/Shalnon/StudyingVulkan/tree/simpleTriangle "Simple Triangle")
 
#### Screenshot from current example
![4xmsaa](https://drive.google.com/uc?export=view&id=1QA2deLGV-JO_kfmv_AvpPDvS3FfXR3KG)  
 
## Project Goals
This project was created with a couple main goals in mind:
   * Create one of the easiest to read piece of non-trivial vulkan code out there.
   * Establish a commenting style that makes it easier to search for where specific vulkan parameters are initalized, and to simplify finding the relevant region of code when a validation error/warning is hit

```
// Example
    VkPipelineVertexInputStateCreateInfo   vertexInputStateCreateInfo =
    {
       /*..VkStructureType.............................sType.............................*/ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
       /*..const.void*.................................pNext.............................*/ nullptr,
       /*..VkPipelineVertexInputStateCreateFlags.......flags.............................*/ 0, // Reserved for future use as of vulkan 1.3
       /*..uint32_t....................................vertexBindingDescriptionCount.....*/ numVertexInputBindings,
       /*..const.VkVertexInputBindingDescription*......pVertexBindingDescriptions........*/ pVertexInputBindingDescriptions,
       /*..uint32_t....................................vertexAttributeDescriptionCount...*/ numVertexAttributes,
       /*..const.VkVertexInputAttributeDescription*....pVertexAttributeDescriptions......*/ pInputAttributeDescriptions
    };
```
   * Use a predominately functional programming model to keep the callstack shallow and reduce the amount of cross rerferencing necessary to understand the purpsoe and intention behind a piece of code.
   * Take advantage of subpasses for deferred rendering instead of a series of multiple renderpasses.
   * Use the swapchain and Microsoft windowing system more directly, and avoid using libraries such as GLFW.

## Usage
   - Press arrow keys to rotate scene
   - Press escape key to close application

## Requirements
   * Windows
   * Vulkan 1.3
   * Vulkan SDK

## Build
   This project uses cmake.

* Note that 3rd party libraries are automatically downloaded and linked via cmakes's FetchContent functionality at cmake configure time:
```
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
```

## Issues
   * Background is always black
      * Clear color seems to be discarded when rendering using full screen quad in second subpass.

Future goals
  * Futher limit number of resources.
   * Use only 2 sets of swapchain image resources instead of creating as many sets of resources as their are available swapchain images. (assuming that number is > 2)
   * There likely doesnt need to be as many staging buffer as there are. Creating a smaller number of staging buffers and structuring the code to reuse them would better.
  * Add texuring support so that diffuse color GBUFFER images include data read from texutre reads rather than a static material specific color value.
  * Add normal map support so that the surface normals GBUFFER Image can use based on more than per-triangle normal data.
  * Address remaining @todo items
  * Fix messed up normals present in the model file.
