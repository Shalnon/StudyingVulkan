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
    *subpass0DescriptorSetLayout = dSet1Layout;

    return result;
}



VkDescriptorPool CreateDescriptorPool (VkDevice logicalDevice)
{
    VkResult         result               = VK_INCOMPLETE;
    VkDescriptorPool descriptorPoolHandle = VK_NULL_HANDLE;

    const uint32_t numDescriptorTypes            = SceneVulkanParameters::numDescriptorTypesUsedInScene;
    const uint32_t numUboDescriptors             = SceneVulkanParameters::numUboDescriptorsInScene;
    const uint32_t numSsboDescriptors            = SceneVulkanParameters::numSsboDescriptorsInScene;
    const uint32_t numInputAttachmentDescriptors = SceneVulkanParameters::numInputAttachmentDescriptorsInScene;


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
        /*...uint32_t.......................maxSets.........*/ SceneVulkanParameters::numDescriporSets,
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

//@TODO: factor this out. not really needed. move contents to the place its called
VkDescriptorSet AllocateDescriptorSet (VkDevice               logicalDevice,
                                       VkDescriptorPool       descriptorPool,
                                       VkDescriptorSetLayout  descriptorSetLayout)
{
    assert (logicalDevice       != VK_NULL_HANDLE);
    assert (descriptorPool      != VK_NULL_HANDLE);
    assert (descriptorSetLayout != VK_NULL_HANDLE);

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo =
    {
        /*...VkStructureType.................sType................*/ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        /*...const.void*.....................pNext................*/ nullptr,
        /*...VkDescriptorPool................descriptorPool.......*/ descriptorPool,
        /*...uint32_t........................descriptorSetCount...*/ NUM_DESCRIPTOR_SETS,
        /*...const.VkDescriptorSetLayout*....pSetLayouts..........*/ &descriptorSetLayout
    };

    VkDescriptorSet descriptorSetHandle = VK_NULL_HANDLE;
    VkResult        result              = vkAllocateDescriptorSets(logicalDevice,
                                                                   &descriptorSetAllocateInfo,
                                                                   &descriptorSetHandle);

    assert (descriptorSetHandle != VK_NULL_HANDLE);
    assert (result              == VK_SUCCESS    );

    return descriptorSetHandle;
}
#endif // !VULKAN_DESCRIPTOR_TOOLS_CPP
