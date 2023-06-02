//glslc --target-env=vulkan1.3 -fshader-stage=vert -o vert.spv triangle.vert 

#version 450
precision highp float;

layout(location = 0) in  vec3 inPosition;
layout(location = 0) out vec3 out_color;

layout( binding = 0) uniform UniformBufferObject
{
    mat4 sceneTransform;
    vec4 sceneScale;
} ubo;

layout( binding = 1) readonly buffer MaterialsSSBO
{
    vec4 colors[];
} ssbo;

void main()
{
    mat4 meshTransform = ubo.sceneTransform;

    vec4 vpos = vec4(inPosition.xyz, 1.0) * meshTransform;
    gl_Position = vpos * ubo.sceneScale;

    out_color = ssbo.colors[gl_InstanceIndex].rgb;
}
