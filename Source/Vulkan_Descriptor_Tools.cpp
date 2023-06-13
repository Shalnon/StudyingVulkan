#ifndef VULKAN_DESCRIPTOR_TOOLS_CPP
#define VULKAN_DESCRIPTOR_TOOLS_CPP

#include "Vulkan_Descriptor_Tools.h"


VkResult CreateDescriptorSetLayout (VkDevice logicalDevice,
                                    VkDescriptorSetLayout* subpass0DescriptorSetLayout,
                                    VkDescriptorSetLayout* subpass1DescriptorSetLayout)
{
    VkDescriptorSetLayout dSet0Layout = VK_NULL_HANDLE;
    VkDescriptorSetLayout dSet1Layout = VK_NULL_HANDLE;
    VkResult              result     = VK_INCOMPLETE;

    /******  From Vertex Shader   *******
    * layout( binding = 0) uniform UniformBufferObject
    * {
    *     // Scene Transform
    *     mat4 sceneTransform;
    *     vec4 sceneScale;
    * 
    *     // Scene ambient color
    *     float ambient_Coefficient;
    *     vec3  ambient_color;
    * 
    *     // Light location
    *     vec3 lightLocation;
    * } ubo;
    *******************************************/

    const uint32_t numSubpass0LayoutBindings = SceneVulkanParameters::Subpass0::numDescriptorSetLayoutBindings; //2; // Subpass 0: uses an SSBO and a UBO
    const uint32_t numSubpass1LayoutBindings = SceneVulkanParameters::Subpass1::numDescriptorSetLayoutBindings; // Subpass 1: Uses 3 input attachments and the same ubo

    VkDescriptorSetLayoutBinding pDescriptorSet0BindingsLayout[numSubpass0LayoutBindings] =
    {
        {
            /*...uint32_t..............binding................*/ SceneVulkanParameters::Subpass0::uniformBufferBinding, // 0
            /*...VkDescriptorType......descriptorType.........*/ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /*...uint32_t..............descriptorCount........*/ 1, // This would be greater than 1 if ubo was declared in the shader like }ubo[n]; where n>1
            /*...VkShaderStageFlags....stageFlags.............*/ VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            /*...const.VkSampler*......pImmutableSamplers.....*/ nullptr
        },
        {
            /*...uint32_t..............binding................*/ SceneVulkanParameters::Subpass0::storageBufferBinding, // 1
            /*...VkDescriptorType......descriptorType.........*/ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            /*...uint32_t..............descriptorCount........*/ 1,
            /*...VkShaderStageFlags....stageFlags.............*/ VK_SHADER_STAGE_VERTEX_BIT,
            /*...const.VkSampler*......pImmutableSamplers.....*/ nullptr
        }
    };

    VkDescriptorSetLayoutBinding pDescriptorSet1BindingsLayout[numSubpass1LayoutBindings] =
    {
        {
            /*...uint32_t..............binding................*/ SceneVulkanParameters::Subpass1::uniformBufferBinding, // 0
            /*...VkDescriptorType......descriptorType.........*/ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /*...uint32_t..............descriptorCount........*/ 1, // This would be greater than 1 if ubo was declared in the shader like }ubo[n]; where n>1
            /*...VkShaderStageFlags....stageFlags.............*/ VK_SHADER_STAGE_FRAGMENT_BIT,
            /*...const.VkSampler*......pImmutableSamplers.....*/ nullptr
        },
        {
            /*...uint32_t..............binding................*/ SceneVulkanParameters::Subpass1::diffuseColorInputAttachmentBinding, // 1
            /*...VkDescriptorType......descriptorType.........*/ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            /*...uint32_t..............descriptorCount........*/ 1,  //@todo: Look into whether there are any advantages to using an input attachment array with one descriptor.
            /*...VkShaderStageFlags....stageFlags.............*/ VK_SHADER_STAGE_FRAGMENT_BIT,
            /*...const.VkSampler*......pImmutableSamplers.....*/ nullptr
        },
        {
            /*...uint32_t..............binding................*/ SceneVulkanParameters::Subpass1::normalVectorInputAttachmentBinding, // 2
            /*...VkDescriptorType......descriptorType.........*/ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            /*...uint32_t..............descriptorCount........*/ 1,
            /*...VkShaderStageFlags....stageFlags.............*/ VK_SHADER_STAGE_FRAGMENT_BIT,
            /*...const.VkSampler*......pImmutableSamplers.....*/ nullptr
        },
        {
            /*...uint32_t..............binding................*/ SceneVulkanParameters::Subpass1::depthImageInputAttachmentBinding, // 3
            /*...VkDescriptorType......descriptorType.........*/ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            /*...uint32_t..............descriptorCount........*/ 1,
            /*...VkShaderStageFlags....stageFlags.............*/ VK_SHADER_STAGE_FRAGMENT_BIT,
            /*...const.VkSampler*......pImmutableSamplers.....*/ nullptr
        }
    };
    
    VkDescriptorSetLayoutCreateInfo dSet0LayoutCreateInfo =
    {
        /*...VkStructureType........................sType..........*/ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        /*...const.void*............................pNext..........*/ nullptr,
        /*...VkDescriptorSetLayoutCreateFlags.......flags..........*/ 0,
        /*...uint32_t...............................bindingCount...*/ numSubpass0LayoutBindings,
        /*...const.VkDescriptorSetLayoutBinding*....pBindings......*/ pDescriptorSet0BindingsLayout
    };

    VkDescriptorSetLayoutCreateInfo dSet1LayoutCreateInfo =
    {
        /*...VkStructureType........................sType..........*/ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        /*...const.void*............................pNext..........*/ nullptr,
        /*...VkDescriptorSetLayoutCreateFlags.......flags..........*/ 0,
        /*...uint32_t...............................bindingCount...*/ numSubpass1LayoutBindings,
        /*...const.VkDescriptorSetLayoutBinding*....pBindings......*/ pDescriptorSet1BindingsLayout
    };

    result = vkCreateDescriptorSetLayout (logicalDevice, &dSet0LayoutCreateInfo, nullptr, &dSet0Layout);
    assert (result == VK_SUCCESS);

    result = vkCreateDescriptorSetLayout (logicalDevice, &dSet1LayoutCreateInfo, nullptr, &dSet1Layout);
    assert (result == VK_SUCCESS);

    *subpass0DescriptorSetLayout = dSet0Layout;
    *subpass1DescriptorSetLayout = dSet1Layout;

    return result;
}



VkDescriptorPool CreateDescriptorPool (VkDevice logicalDevice)
{
    VkResult         result               = VK_INCOMPLETE;
    VkDescriptorPool descriptorPoolHandle = VK_NULL_HANDLE;

    const uint32_t numDescriptorTypes            = SceneVulkanParameters::numDescriptorTypesUsedInScene;
    const uint32_t numUboDescriptors             = 3 * SceneVulkanParameters::numUboDescriptorsInScene;
    const uint32_t numSsboDescriptors            = 3 * SceneVulkanParameters::numSsboDescriptorsInScene;
    const uint32_t numInputAttachmentDescriptors = 3 * SceneVulkanParameters::numInputAttachmentDescriptorsInScene;


    VkDescriptorPoolSize pDescriptorPoolSizes[numDescriptorTypes] =
    {
        {
            /*...VkDescriptorType....type................*/ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /*...uint32_t............descriptorCount.....*/ numUboDescriptors
        },
        {
            /*...VkDescriptorType....type................*/ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
            /*...uint32_t............descriptorCount.....*/ numSsboDescriptors
        },
        {
            /*...VkDescriptorType....type................*/ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,
            /*...uint32_t............descriptorCount.....*/ numInputAttachmentDescriptors
        }
    };

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo  =
    {
        /*...VkStructureType................sType...........*/ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        /*...const.void*....................pNext...........*/ nullptr,
        /*...VkDescriptorPoolCreateFlags....flags...........*/ 0,
        /*...uint32_t.......................maxSets.........*/ SceneVulkanParameters::totalNumDescriporSets * 3,
        /*...uint32_t.......................poolSizeCount...*/ numDescriptorTypes, // Using one VkDescriptorPoolSize struct per descriptor type
        /*...const.VkDescriptorPoolSize*....pPoolSizes......*/ pDescriptorPoolSizes
    };

    result = vkCreateDescriptorPool (logicalDevice,
                                     &descriptorPoolCreateInfo,
                                     nullptr,
                                     &descriptorPoolHandle);

    assert (result               == VK_SUCCESS    );
    assert (descriptorPoolHandle != VK_NULL_HANDLE);

    return  descriptorPoolHandle;
}



VkPipeline CreateSubpass0Pipeline (VkDevice               logicalDevice,
                                   VkRenderPass           renderpass,
                                   VkExtent2D* pExtent,
                                   VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                   const char* fragShaderPath,
                                   const char* vertShaderPath,
                                   VkPipelineLayout* pPipelineLayoutHandleOut)
{
    uint32_t numVertexAttributes = sizeof (s_VertexShaderAttributes) / sizeof (AttributeInfo);
    assert (numVertexAttributes > 0);

    const uint32_t VERTEX_ATTRIB_POSITION_IDX = SceneVulkanParameters::Subpass0::vertexLocationAttributePosition;
    const uint32_t VERTEX_ATTRIB_NORMAL_IDX   = SceneVulkanParameters::Subpass0::vertexNormalAttributePosition;

    VkVertexInputAttributeDescription pInputAttributeDescriptions[] =
    {
        {
            /*...uint32_t....location....*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].attributeIdx,
            /*...uint32_t....binding.....*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].bufferBindingIdx,
            /*...VkFormat....format......*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].format,
            /*...uint32_t....offset......*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].offset
        },
        {
            /*...uint32_t....location....*/ s_VertexShaderAttributes[VERTEX_ATTRIB_NORMAL_IDX].attributeIdx,
            /*...uint32_t....binding.....*/ s_VertexShaderAttributes[VERTEX_ATTRIB_NORMAL_IDX].bufferBindingIdx,
            /*...VkFormat....format......*/ s_VertexShaderAttributes[VERTEX_ATTRIB_NORMAL_IDX].format,
            /*...uint32_t....offset......*/ s_VertexShaderAttributes[VERTEX_ATTRIB_NORMAL_IDX].offset
        }
    };
    assert ((sizeof (pInputAttributeDescriptions) / sizeof (VkVertexInputAttributeDescription)) == numVertexAttributes);

    // One binding because all vertex attributes are in the same buffer. Thus only one buffer needs a binding.
    uint32_t numInputBindings = 1;
    VkVertexInputBindingDescription pVertexInputBindingDescriptions[] =
    {
        {
            /*..uint32_t.............binding......*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].bufferBindingIdx,
            /*..uint32_t.............stride.......*/ s_VertexShaderAttributes[VERTEX_ATTRIB_POSITION_IDX].stride +
                                                     s_VertexShaderAttributes[VERTEX_ATTRIB_NORMAL_IDX].stride,
        /*..VkVertexInputRate....inputRate....*/ VK_VERTEX_INPUT_RATE_VERTEX
    }
    };
    assert ((sizeof (pVertexInputBindingDescriptions) / sizeof (VkVertexInputBindingDescription)) == numInputBindings);

    VkPipelineVertexInputStateCreateInfo   vertexInputStateCreateInfo =
    {
        /*...VkStructureType.............................sType.............................*/ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        /*...const.void*.................................pNext.............................*/ nullptr,
        /*...VkPipelineVertexInputStateCreateFlags.......flags.............................*/ 0, // Reserved for future use as of vulkan 1.3
        /*...uint32_t....................................vertexBindingDescriptionCount.....*/ numInputBindings,
        /*...const.VkVertexInputBindingDescription*......pVertexBindingDescriptions........*/ pVertexInputBindingDescriptions,
        /*...uint32_t....................................vertexAttributeDescriptionCount...*/ numVertexAttributes,
        /*...const.VkVertexInputAttributeDescription*....pVertexAttributeDescriptions......*/ pInputAttributeDescriptions
    };

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo =
    {
        /*....VkStructureType............................sType......................*/ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        /*....const.void*................................pNext......................*/ 0,
        /*....VkPipelineInputAssemblyStateCreateFlags....flags......................*/ 0,
        /*....VkPrimitiveTopology........................topology...................*/ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        /*....VkBool32...................................primitiveRestartEnable.....*/ VK_FALSE
    };

    // The pipeline layout is used to register resources like UBOs, samplers, SSBOs and other resources requiring a descriptor set to be used
    VkPipelineLayout           pipelineLayout = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo layout0CreateInfo =
    {
        /*...VkStructureType.................sType......................*/ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        /*...const.void*.....................pNext......................*/ nullptr,
        /*...VkPipelineLayoutCreateFlags.....flags......................*/ 0,
        /*...uint32_t........................setLayoutCount.............*/ 1,
        /*...const.VkDescriptorSetLayout*....pSetLayouts................*/ &descriptorSetLayoutHandle,
        /*...uint32_t........................pushConstantRangeCount.....*/ 0,
        /*...const.VkPushConstantRange*......pPushConstantRanges........*/ nullptr
    };

    //@VKSPEC: "Access to descriptor sets from a pipeline is accomplished through a pipeline layout." : https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPipelineLayout.html
    VkResult result = vkCreatePipelineLayout (logicalDevice, &layout0CreateInfo, 0, &pipelineLayout);
    assert (result == VK_SUCCESS);


    *pPipelineLayoutHandleOut = pipelineLayout;// pPipelineLayouts[0];
   // pPipelineLayoutHandleOut[1] = pPipelineLayouts[1]; //This doesnt really work.  Move all this into its own func for the second subpass/pipeline

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo =
    {
        /*...VkStructureType............................sType......................*/ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        /*...const.void*................................pNext......................*/ 0,
        /*...VkPipelineRasterizationStateCreateFlags....flags......................*/ 0,
        /*...VkBool32...................................depthClampEnable...........*/ VK_FALSE,
        /*...VkBool32...................................rasterizerDiscardEnable....*/ VK_FALSE,
        /*...VkPolygonMode..............................polygonMode................*/ VK_POLYGON_MODE_FILL,
        /*...VkCullModeFlags............................cullMode...................*/ VK_CULL_MODE_NONE,
        /*...VkFrontFace................................frontFace..................*/ VK_FRONT_FACE_CLOCKWISE,
        /*...VkBool32...................................depthBiasEnable............*/ VK_FALSE,
        /*...float......................................depthBiasConstantFactor....*/ 0.0,
        /*...float......................................depthBiasClamp.............*/ 0.0,
        /*...float......................................depthBiasSlopeFactor.......*/ 0.0,
        /*...float......................................lineWidth..................*/ 1.0f
    };

    // From Khronos sample: | Our attachment will write to all color channels, but no blending is enabled.  |
    VkPipelineColorBlendAttachmentState pColorBlendAttachmentState[SceneVulkanParameters::Subpass0::numColorAttachments] = {};
    pColorBlendAttachmentState[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pColorBlendAttachmentState[1].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    pColorBlendAttachmentState[2].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo blendState =
    {
        /*...VkStructureType...............................sType.................*/ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        /*...const.void*...................................pNext.................*/ 0,
        /*...VkPipelineColorBlendStateCreateFlags..........flags.................*/ 0,
        /*...VkBool32......................................logicOpEnable.........*/ VK_FALSE,
        /*...VkLogicOp.....................................logicOp...............*/ (VkLogicOp)0, // This value only used if logicOpEnable is VK_TRUE
        /*...uint32_t......................................attachmentCount.......*/ SceneVulkanParameters::Subpass0::numColorAttachments,
        /*...const.VkPipelineColorBlendAttachmentState*....pAttachments..........*/ pColorBlendAttachmentState,
        /*...float.........................................blendConstants[4].....*/ 0
    };

    VkRect2D scissor =
    {
        /*...VkOffset2D....offset....*/ { 0, 0 },
        /*...VkExtent2D....extent....*/ {pExtent->width, pExtent->height}
    };

    VkViewport viewport =
    {
        /*...float....x.........*/ 0,
        /*...float....y.........*/ 0,
        /*...float....width.....*/ float (pExtent->width),
        /*...float....height....*/ float (pExtent->height),
        /*...float....minDepth..*/ 0.0f,
        /*...float....maxDepth..*/ 1.0f
    };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo =
    {
        /*...VkStructureType.......................sType............*/ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        /*...const.void*...........................pNext............*/ 0,
        /*...VkPipelineViewportStateCreateFlags....flags............*/ 0, //reserved for future use as of vk1.3
        /*...uint32_t..............................viewportCount....*/ 1,
        /*...const.VkViewport*.....................pViewports.......*/ &viewport,
        /*...uint32_t..............................scissorCount.....*/ 1,
        /*...const.VkRect2D*.......................pScissors........*/ &scissor
    };

    // Disabling all depth testing for now
    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo =
    {
        /*...VkStructureType...........................sType..................*/ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        /*...const.void*...............................pNext..................*/ 0,
        /*...VkPipelineDepthStencilStateCreateFlags....flags..................*/ 0,
        /*...VkBool32..................................depthTestEnable........*/ VK_TRUE, //@TODO: check if i need to disable for second pipeline?
        /*...VkBool32..................................depthWriteEnable.......*/ VK_TRUE, //
        /*...VkCompareOp...............................depthCompareOp.........*/ VK_COMPARE_OP_GREATER,
        /*...VkBool32..................................depthBoundsTestEnable..*/ VK_TRUE,
        /*...VkBool32..................................stencilTestEnable......*/ VK_FALSE,
        /*...VkStencilOpState..........................front..................*/ {},
        /*...VkStencilOpState..........................back...................*/ {},
        /*...float.....................................minDepthBounds.........*/ viewport.minDepth,//0.0f,
        /*...float.....................................maxDepthBounds.........*/ viewport.maxDepth //1.0f
    };

    // No Multisampling. 
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo =
    {
        /*...VkStructureType..........................sType.....................*/ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        /*...const.void*..............................pNext.....................*/ 0,
        /*...VkPipelineMultisampleStateCreateFlags....flags.....................*/ 0,
        /*...VkSampleCountFlagBits....................rasterizationSamples......*/ VK_SAMPLE_COUNT_1_BIT,
        /*...VkBool32.................................sampleShadingEnable.......*/ VK_FALSE,
        /*...float....................................minSampleShading..........*/ 0.0,      // Dont think this is used unless sampleShadingEnable is true. 
        /*...const.VkSampleMask*......................pSampleMask...............*/ 0,        // @VK_SPEC: If pSampleMask is NULL, it is treated as if the mask has all bits set to 1.
        /*...VkBool32.................................alphaToCoverageEnable.....*/ false,
        /*...VkBool32.................................alphaToOneEnable..........*/ VK_FALSE  // @VK_SPEC: "If the alphaToOne feature is not enabled, alphaToOneEnable must be VK_FALSE"
    };

    // From the khronos sample: | Specify that these states will be dynamic, i.e. not part of pipeline state object. | //@TODO: look into if i actually want to do this. less dynamic state is probably better...
    VkDynamicState dynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo =
    {
        /*...VkStructureType......................sType..................*/ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        /*...const.void*..........................pNext..................*/ 0,
        /*...VkPipelineDynamicStateCreateFlags....flags..................*/ 0,
        /*...uint32_t.............................dynamicStateCount......*/ 2,
        /*...const.VkDynamicState*................pDynamicStates.........*/ dynamicStates
    };

    static const uint32_t vertexShaderIdx   = 0;
    static const uint32_t fragmentShaderIdx = 1;
    static const uint32_t numStages         = 2;

    VkShaderModule pShaderModules[2] = { VK_NULL_HANDLE, VK_NULL_HANDLE };

    pShaderModules[fragmentShaderIdx] = CreateShaderModule (logicalDevice, fragShaderPath, true, false);
    pShaderModules[vertexShaderIdx]   = CreateShaderModule (logicalDevice, vertShaderPath, false, true);

    VkPipelineShaderStageCreateInfo pShaderStageCreateInfos[numStages] =
    {
        {
            /*..VkStructureType.....................sType....................*/ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            /*..const.void*.........................pNext....................*/ 0,
            /*..VkPipelineShaderStageCreateFlags....flags....................*/ 0,
            /*..VkShaderStageFlagBits...............stage....................*/ VK_SHADER_STAGE_VERTEX_BIT,
            /*..VkShaderModule......................module...................*/ pShaderModules[vertexShaderIdx],
            /*..const.char*.........................pName....................*/ "main",
            /*..const.VkSpecializationInfo*.........pSpecializationInfo......*/ 0,
        },
        {
            /*..VkStructureType.....................sType....................*/ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            /*..const.void*.........................pNext....................*/ 0,
            /*..VkPipelineShaderStageCreateFlags....flags....................*/ 0,
            /*..VkShaderStageFlagBits...............stage....................*/ VK_SHADER_STAGE_FRAGMENT_BIT,
            /*..VkShaderModule......................module...................*/ pShaderModules[fragmentShaderIdx],
            /*..const.char*.........................pName....................*/ "main",
            /*..const.VkSpecializationInfo*.........pSpecializationInfo......*/ 0
        }
    };

    assert (pShaderModules[vertexShaderIdx] != VK_NULL_HANDLE);
    assert (pShaderModules[fragmentShaderIdx] != VK_NULL_HANDLE);
    assert (renderpass != VK_NULL_HANDLE);
    assert (*pPipelineLayoutHandleOut != VK_NULL_HANDLE);

    VkGraphicsPipelineCreateInfo gfxPipelineCreateInfo =
    {
        /*...VkStructureType..................................sType...................*/ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        /*...const.void*......................................pNext...................*/ 0,
        /*...VkPipelineCreateFlags............................flags...................*/ 0,
        /*...uint32_t.........................................stageCount..............*/ numStages,
        /*...const.VkPipelineShaderStageCreateInfo*...........pStages.................*/ pShaderStageCreateInfos,
        /*...const.VkPipelineVertexInputStateCreateInfo*......pVertexInputState.......*/ &vertexInputStateCreateInfo,
        /*...const.VkPipelineInputAssemblyStateCreateInfo*....pInputAssemblyState.....*/ &pipelineInputAssemblyStateCreateInfo,
        /*...const.VkPipelineTessellationStateCreateInfo*.....pTessellationState......*/ nullptr,
        /*...const.VkPipelineViewportStateCreateInfo*.........pViewportState..........*/ &viewportStateCreateInfo,
        /*...const.VkPipelineRasterizationStateCreateInfo*....pRasterizationState.....*/ &rasterizationStateCreateInfo,
        /*...const.VkPipelineMultisampleStateCreateInfo*......pMultisampleState.......*/ &multisampleStateCreateInfo,
        /*...const.VkPipelineDepthStencilStateCreateInfo*.....pDepthStencilState......*/ &depthStencilCreateInfo,
        /*...const.VkPipelineColorBlendStateCreateInfo*.......pColorBlendState........*/ &blendState,
        /*...const.VkPipelineDynamicStateCreateInfo*..........pDynamicState...........*/ &dynamicStateCreateInfo,
        /*...VkPipelineLayout.................................layout..................*/ pipelineLayout,
        /*...VkRenderPass.....................................renderPass..............*/ renderpass,
        /*...uint32_t.........................................subpass.................*/ 0,
        /*...VkPipeline.......................................basePipelineHandle......*/ 0,
        /*...int32_t..........................................basePipelineIndex.......*/ 0
    };

    VkPipeline gfxPipeline = VK_NULL_HANDLE;

    // Create the graphics pipeline object
    result = vkCreateGraphicsPipelines (/*..VkDevice.............................device............*/ logicalDevice,
                                        /*..VkPipelineCache......................pipelineCache.....*/ VK_NULL_HANDLE,
                                        /*..uint32_t.............................createInfoCount...*/ 1,
                                        /*..const.VkGraphicsPipelineCreateInfo*..pCreateInfos......*/ &gfxPipelineCreateInfo,
                                        /*..const.VkAllocationCallbacks*.........pAllocator........*/ nullptr,
                                        /*..VkPipeline*..........................pPipelines........*/ &gfxPipeline);
    assert (gfxPipeline != VK_NULL_HANDLE);

    vkDestroyShaderModule (logicalDevice, pShaderModules[vertexShaderIdx], nullptr);
    vkDestroyShaderModule (logicalDevice, pShaderModules[fragmentShaderIdx], nullptr);

    return gfxPipeline;
}


VkPipeline CreateSubpass1Pipeline (VkDevice               logicalDevice,
                                   VkRenderPass           renderpass,
                                   VkExtent2D*            pExtent,
                                   VkDescriptorSetLayout  descriptorSetLayoutHandle,
                                   const char*            fragShaderPath,
                                   const char*            vertShaderPath,
                                   VkPipelineLayout*      pPipelineLayoutHandleOut)
{

    VkPipelineVertexInputStateCreateInfo   vertexInputStateCreateInfo =
    {
        /*...VkStructureType.............................sType.............................*/ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        /*...const.void*.................................pNext.............................*/ 0,
        /*...VkPipelineVertexInputStateCreateFlags.......flags.............................*/ 0,
        /*...uint32_t....................................vertexBindingDescriptionCount.....*/ 0,
        /*...const.VkVertexInputBindingDescription*......pVertexBindingDescriptions........*/ nullptr,
        /*...uint32_t....................................vertexAttributeDescriptionCount...*/ 0,
        /*...const.VkVertexInputAttributeDescription*....pVertexAttributeDescriptions......*/ nullptr
    };

    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo =
    {
        /*....VkStructureType............................sType......................*/ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        /*....const.void*................................pNext......................*/ 0,
        /*....VkPipelineInputAssemblyStateCreateFlags....flags......................*/ 0,
        /*....VkPrimitiveTopology........................topology...................*/ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        /*....VkBool32...................................primitiveRestartEnable.....*/ VK_FALSE
    };

    VkPipelineLayout                       pipelineLayout   = VK_NULL_HANDLE;
    VkPipelineLayoutCreateInfo             layoutCreateInfo =
    {
        /*...VkStructureType.................sType......................*/ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        /*...const.void*.....................pNext......................*/ nullptr,
        /*...VkPipelineLayoutCreateFlags.....flags......................*/ 0,
        /*...uint32_t........................setLayoutCount.............*/ 1,
        /*...const.VkDescriptorSetLayout*....pSetLayouts................*/ &descriptorSetLayoutHandle,
        /*...uint32_t........................pushConstantRangeCount.....*/ 0,
        /*...const.VkPushConstantRange*......pPushConstantRanges........*/ nullptr
    };


    //@VKSPEC: "Access to descriptor sets from a pipeline is accomplished through a pipeline layout." : https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPipelineLayout.html
    VkResult result = vkCreatePipelineLayout (logicalDevice, &layoutCreateInfo, 0, &pipelineLayout);
    assert (result == VK_SUCCESS);

    *pPipelineLayoutHandleOut = pipelineLayout;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo =
    {
        /*...VkStructureType............................sType......................*/ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        /*...const.void*................................pNext......................*/ 0,
        /*...VkPipelineRasterizationStateCreateFlags....flags......................*/ 0,
        /*...VkBool32...................................depthClampEnable...........*/ VK_FALSE,
        /*...VkBool32...................................rasterizerDiscardEnable....*/ VK_FALSE,
        /*...VkPolygonMode..............................polygonMode................*/ VK_POLYGON_MODE_FILL, //VK_POLYGON_MODE_FILL  = 0, so dont actually need to explicitly set it technically.
        /*...VkCullModeFlags............................cullMode...................*/ VK_CULL_MODE_BACK_BIT,
        /*...VkFrontFace................................frontFace..................*/ VK_FRONT_FACE_CLOCKWISE,
        /*...VkBool32...................................depthBiasEnable............*/ VK_FALSE,
        /*...float......................................depthBiasConstantFactor....*/ 0.0,
        /*...float......................................depthBiasClamp.............*/ 0.0,
        /*...float......................................depthBiasSlopeFactor.......*/ 0.0,
        /*...float......................................lineWidth..................*/ 1.0f
    };

    // From Khronos sample: | Our attachment will write to all color channels, but no blending is enabled.  |
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo blendState =
    {
        /*...VkStructureType...............................sType.................*/ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        /*...const.void*...................................pNext.................*/ 0,
        /*...VkPipelineColorBlendStateCreateFlags..........flags.................*/ 0,
        /*...VkBool32......................................logicOpEnable.........*/ VK_FALSE,
        /*...VkLogicOp.....................................logicOp...............*/ (VkLogicOp)0, // This value only used if logicOpEnable is VK_TRUE
        /*...uint32_t......................................attachmentCount.......*/ 1,
        /*...const.VkPipelineColorBlendAttachmentState*....pAttachments..........*/ &colorBlendAttachmentState,
        /*...float.........................................blendConstants[4].....*/ 0
    };

    VkViewport viewport =
    {
        /*...float....x.........*/ 0,
        /*...float....y.........*/ 0,
        /*...float....width.....*/ float (pExtent->width),
        /*...float....height....*/ float (pExtent->height),
        /*...float....minDepth..*/ 0.0f,
        /*...float....maxDepth..*/ 1.0f
    };

    VkRect2D scissor =
    {
        /*...VkOffset2D....offset....*/ { 0.0f, 0.0},
        /*...VkExtent2D....extent....*/ {pExtent->width, pExtent->height}
    };

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo =
    {
        /*...VkStructureType.......................sType............*/ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        /*...const.void*...........................pNext............*/ 0,
        /*...VkPipelineViewportStateCreateFlags....flags............*/ 0, //reserved for future use as of vk1.3
        /*...uint32_t..............................viewportCount....*/ 1,
        /*...const.VkViewport*.....................pViewports.......*/ &viewport, //@NOTE: Why is this not set in the khronos example? spec says this member is ignored if 
        /*...uint32_t..............................scissorCount.....*/ 1,
        /*...const.VkRect2D*.......................pScissors........*/ &scissor
    };

    // Disabling all depth testing for now
    VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo =
    {
        /*...VkStructureType...........................sType..................*/ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        /*...const.void*...............................pNext..................*/ 0,
        /*...VkPipelineDepthStencilStateCreateFlags....flags..................*/ 0,
        /*...VkBool32..................................depthTestEnable........*/ VK_FALSE,
        /*...VkBool32..................................depthWriteEnable.......*/ VK_FALSE,
        /*...VkCompareOp...............................depthCompareOp.........*/ VK_COMPARE_OP_NEVER, // VK_COMPARE_OP_NEVER  == 0
        /*...VkBool32..................................depthBoundsTestEnable..*/ VK_FALSE,
        /*...VkBool32..................................stencilTestEnable......*/ VK_FALSE,
        /*...VkStencilOpState..........................front..................*/ {},
        /*...VkStencilOpState..........................back...................*/ {},
        /*...float.....................................minDepthBounds.........*/ 0.0f,
        /*...float.....................................maxDepthBounds.........*/ 0.0f
    };

    // No Multisampling. 
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo =
    {
        /*...VkStructureType..........................sType.....................*/ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        /*...const.void*..............................pNext.....................*/ 0,
        /*...VkPipelineMultisampleStateCreateFlags....flags.....................*/ 0,
        /*...VkSampleCountFlagBits....................rasterizationSamples......*/ VK_SAMPLE_COUNT_1_BIT,
        /*...VkBool32.................................sampleShadingEnable.......*/ VK_FALSE,
        /*...float....................................minSampleShading..........*/ 0.0,      // Dont think this is used unless sampleShadingEnable is true. 
        /*...const.VkSampleMask*......................pSampleMask...............*/ 0,        // @VK_SPEC: If pSampleMask is NULL, it is treated as if the mask has all bits set to 1.
        /*...VkBool32.................................alphaToCoverageEnable.....*/ false,
        /*...VkBool32.................................alphaToOneEnable..........*/ VK_FALSE  // @VK_SPEC: "If the alphaToOne feature is not enabled, alphaToOneEnable must be VK_FALSE"
    };

    // From the khronos sample: | Specify that these states will be dynamic, i.e. not part of pipeline state object. |
    VkDynamicState dynamicStates[2] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo =
    {
        /*...VkStructureType......................sType..................*/ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        /*...const.void*..........................pNext..................*/ 0,
        /*...VkPipelineDynamicStateCreateFlags....flags..................*/ 0,
        /*...uint32_t.............................dynamicStateCount......*/ 2,
        /*...const.VkDynamicState*................pDynamicStates.........*/ dynamicStates
    };

    static const uint32_t vertexShaderIdx = 0;
    static const uint32_t fragmentShaderIdx = 1;
    static const uint32_t numStages = 2;

    VkShaderModule pShaderModules[2];

    pShaderModules[fragmentShaderIdx] = CreateShaderModule (logicalDevice, fragShaderPath, true, false);
    pShaderModules[vertexShaderIdx] = CreateShaderModule (logicalDevice, vertShaderPath, false, true);


    VkPipelineShaderStageCreateInfo pShaderStageCreateInfos[numStages] =
    {
        {
            /*..VkStructureType.....................sType....................*/ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            /*..const.void*.........................pNext....................*/ 0,
            /*..VkPipelineShaderStageCreateFlags....flags....................*/ 0,
            /*..VkShaderStageFlagBits...............stage....................*/ VK_SHADER_STAGE_VERTEX_BIT,
            /*..VkShaderModule......................module...................*/ pShaderModules[vertexShaderIdx],
            /*..const.char*.........................pName....................*/ "main",
            /*..const.VkSpecializationInfo*.........pSpecializationInfo......*/ 0,
        },
        {
            /*..VkStructureType.....................sType....................*/ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            /*..const.void*.........................pNext....................*/ 0,
            /*..VkPipelineShaderStageCreateFlags....flags....................*/ 0,
            /*..VkShaderStageFlagBits...............stage....................*/ VK_SHADER_STAGE_FRAGMENT_BIT,
            /*..VkShaderModule......................module...................*/ pShaderModules[fragmentShaderIdx],
            /*..const.char*.........................pName....................*/ "main",
            /*..const.VkSpecializationInfo*.........pSpecializationInfo......*/ 0
        }
    };

    assert (pShaderModules[vertexShaderIdx] != VK_NULL_HANDLE);
    assert (pShaderModules[fragmentShaderIdx] != VK_NULL_HANDLE);
    assert (renderpass != VK_NULL_HANDLE);
    assert (pipelineLayout != VK_NULL_HANDLE);

    VkGraphicsPipelineCreateInfo gfxPipelineCreateInfo =
    {
        /*...VkStructureType..................................sType...................*/ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        /*...const.void*......................................pNext...................*/ 0,
        /*...VkPipelineCreateFlags............................flags...................*/ 0,
        /*...uint32_t.........................................stageCount..............*/ numStages,
        /*...const.VkPipelineShaderStageCreateInfo*...........pStages.................*/ pShaderStageCreateInfos,
        /*...const.VkPipelineVertexInputStateCreateInfo*......pVertexInputState.......*/ &vertexInputStateCreateInfo,
        /*...const.VkPipelineInputAssemblyStateCreateInfo*....pInputAssemblyState.....*/ &pipelineInputAssemblyStateCreateInfo,
        /*...const.VkPipelineTessellationStateCreateInfo*.....pTessellationState......*/ nullptr,
        /*...const.VkPipelineViewportStateCreateInfo*.........pViewportState..........*/ &viewportStateCreateInfo,
        /*...const.VkPipelineRasterizationStateCreateInfo*....pRasterizationState.....*/ &rasterizationStateCreateInfo,
        /*...const.VkPipelineMultisampleStateCreateInfo*......pMultisampleState.......*/ &multisampleStateCreateInfo,
        /*...const.VkPipelineDepthStencilStateCreateInfo*.....pDepthStencilState......*/ &depthStencilCreateInfo,
        /*...const.VkPipelineColorBlendStateCreateInfo*.......pColorBlendState........*/ &blendState,
        /*...const.VkPipelineDynamicStateCreateInfo*..........pDynamicState...........*/ &dynamicStateCreateInfo,
        /*...VkPipelineLayout.................................layout..................*/ pipelineLayout,
        /*...VkRenderPass.....................................renderPass..............*/ renderpass,
        /*...uint32_t.........................................subpass.................*/ 1, // This pipeline wil be used in subpass 1 to read input attachments and perform deferred shading
        /*...VkPipeline.......................................basePipelineHandle......*/ 0, // Not using pipeline derivatives
        /*...int32_t..........................................basePipelineIndex.......*/ 0  // Not using pipeline derivatives
    };

    VkPipeline gfxPipeline = VK_NULL_HANDLE;

    // Create the graphics pipeline object
    result = vkCreateGraphicsPipelines (/*..VkDevice.............................device............*/ logicalDevice,
                                        /*..VkPipelineCache......................pipelineCache.....*/ VK_NULL_HANDLE,
                                        /*..uint32_t.............................createInfoCount...*/ 1,
                                        /*..const.VkGraphicsPipelineCreateInfo*..pCreateInfos......*/ &gfxPipelineCreateInfo,
                                        /*..const.VkAllocationCallbacks*.........pAllocator........*/ nullptr,
                                        /*..VkPipeline*..........................pPipelines........*/ &gfxPipeline);
    assert (gfxPipeline != VK_NULL_HANDLE);

    vkDestroyShaderModule (logicalDevice, pShaderModules[vertexShaderIdx], nullptr);
    vkDestroyShaderModule (logicalDevice, pShaderModules[fragmentShaderIdx], nullptr);

    return gfxPipeline;
}
#endif // VULKAN_DESCRIPTOR_TOOLS_CPP
