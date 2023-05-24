#ifndef VULKAN_DESCRIPTOR_TOOLS_CPP
#define VULKAN_DESCRIPTOR_TOOLS_CPP

#include "Vulkan_Descriptor_Tools.h"


VkDescriptorSetLayout CreateDescriptorSetLayout (VkDevice logicalDevice)
{
    VkDescriptorSetLayout dSetLayout = VK_NULL_HANDLE;
    VkResult              result     = VK_INCOMPLETE;

    /******  From Vertex Shader   *******
    * layout(binding = 0) uniform UniformBufferObject
    * {
    *     mat4 modelMatrix;
    *     vec4 sceneScale;
    * } ubo;
    *******************************************/
    VkDescriptorSetLayoutBinding dSetLayoutBinding =
    { 
        /*...uint32_t..............binding................*/ 0,
        /*...VkDescriptorType......descriptorType.........*/ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        /*...uint32_t..............descriptorCount........*/ 1, // This would be greater than 1 if ubo was declared in the shader like }ubo[n]; where n>1
        /*...VkShaderStageFlags....stageFlags.............*/ VK_SHADER_STAGE_VERTEX_BIT,
        /*...const.VkSampler*......pImmutableSamplers.....*/ nullptr 
    };
    
    VkDescriptorSetLayoutCreateInfo dSetLayoutCreateInfo =
    {
        /*...VkStructureType........................sType..........*/ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        /*...const.void*............................pNext..........*/ nullptr,
        /*...VkDescriptorSetLayoutCreateFlags.......flags..........*/ 0,
        /*...uint32_t...............................bindingCount...*/ 1,
        /*...const.VkDescriptorSetLayoutBinding*....pBindings......*/ &dSetLayoutBinding
    };

    result = vkCreateDescriptorSetLayout (logicalDevice, &dSetLayoutCreateInfo, nullptr, &dSetLayout);
    assert (result == VK_SUCCESS);
    
    return dSetLayout;
}



VkDescriptorPool CreateDescriptorPool (VkDevice logicalDevice)
{
    //VkDescriptorTypes          pDescriptorTypesUsed[]= { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
    const uint32_t numUboDescriptorsInScene = 1;
    const uint32_t numDescriptorTypes       = 1;

    VkResult         result               = VK_INCOMPLETE;
    VkDescriptorPool descriptorPoolHandle = VK_NULL_HANDLE;

    VkDescriptorPoolSize pDescriptorPoolSizes[numDescriptorTypes] =
    {
        {
            /*...VkDescriptorType....type................*/ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            /*...uint32_t............descriptorCount.....*/ numUboDescriptorsInScene
        }
//ex... ,{
        //    /*...VkDescriptorType....type................*/// VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        //    /*...uint32_t............descriptorCount.....*/// 1
        //}
    };

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo  =
    {
        /*...VkStructureType................sType...........*/ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        /*...const.void*....................pNext...........*/ nullptr,
        /*...VkDescriptorPoolCreateFlags....flags...........*/ 0,
        /*...uint32_t.......................maxSets.........*/ 1,
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
        /*...uint32_t........................descriptorSetCount...*/ 1,
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
