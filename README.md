README.MD


Vulkan Study - by Sean Halnon

Intro:
  This project uses vulkan to implement a model viewer for rendering a given model using deffered rendering.
  There is also a branch with contains a ultra simple single triangle vulkan example based off of the khronos simple triangle example.

_Branches:_
	This repo contains several different vulkan examples:
		1. Simple Triangle
		2. Deffered Renderer
		3. Deferred Renderer with MSAA (master branch)

   For the most part these branches represent the same project at different points in development. Allowing for easy comparison for how different features are implemented.

_Project Goals_
This project was created with a couple main goals in mind:
   - Create the easiest to read piece of non-trivial vulkan code out there.
       - This vulkan application uses a pradominately functional programming model, rather than an OOP programming model; forcing things to be more explicit.
       - Most of the example vulkan code available at the time of writing this readme seems to prioritize code compactness over readability. Too often, understanding some example code requires keeping track of numerous classes which are defined in an equally numerous number of source files. This can greatly limit how easy it is to fix validation issues do to the constant mental overhead of code navigation when relevant code is in 3+ different files.
       - Use a unique commenting style that allows for API verbage to be more easily linked to the verbage used in variable naming schemes.
          ex: 
          	'''{
                  /*..VkStructureType.............sType................*/ VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, 
                  /*..const.void*.................pNext................*/ 0, 
                  /*..VkDeviceQueueCreateFlags....flags................*/ 0, 
                  /*..uint32_t....................queueFamilyIndex.....*/ chosenQueueFamilyIndex, 
                  /*..uint32_t....................queueCount...........*/ 1, 
                  /*..const.float*................pQueuePriorities.....*/ &queuePriority 
              };'''
        - By avoiding assignment paradigms like "queueInfo.pQueuePriorities = queuePriority", it can be easily and immediately understood that there are no struct members of VkDeviceQueueCreateInfo that are not specifically assigned a value here. This makes it so that the code more often expresses the contents of vulkan parameter structs in their entirety rather than needing to switch to multiple files to be certain of the complete struct contents.

    - Leverage optimal Vulkan API usage patterns for the given rendering approaches.
    	- Basically this means, i try to fully take advantage of subpasses for deferred rendering instead of a series of multiple renderpasses.
    - Use the swapchain and Microsoft windowing system more directly, and avoid using libraries such as GLFW.


_Usage_
   - Press arrow keys to rotate scene
   - Press escape key to close application

_Requirements:_
	- Vulkan 1.3
	- Vulkan SDK

_Build:_
   This project uses cmake.
      - Note that 3rd party libraries are automatically downloaded and linked via cmakes's FetchContent functionality at cmake configure time:

	'''FetchContent_Declare(glm
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
	                     SOURCE_DIR "${EXTERNAL_LIBRARIES_DIRECTORY}/${LIB_VOLK_NAME}")'''

Issues:
   - Background is always black
   	- clear color seems to be discarded when rendering using full screen quad in second subpass.
   - Some geometry glitches out when to close or far away from camera
   	- Likely related to something off with the projection matrix?
	- Need to check GPU for MSAA support. Currently the MSAA example just assumes 4xMSAA is supported.



Future goals
  - Futher limit number of resources.
  	 - Use only 2 sets of swapchain image resources instead of creating as many sets of resources as their are available swapchain images. (assuming that number is > 2)
  	 - There likely doesnt need to be as many staging buffer as there are. Creating a smaller number of staging buffers and structuring the code to reuse them would better.
  - Add texuring support so that diffuse color GBUFFER images include data read from texutre reads rather than a static material specific color value.
  - Add normal map support so that the surface normals GBUFFER Image can use based on more than per-triangle normal data.
  - Address remaining @todo items
  - Create an improved color scheme for wrecking crane model