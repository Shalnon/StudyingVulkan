//glslc --target-env=vulkan1.3 -fshader-stage=vert -o vert.spv subpass0.vert 

#version 450
precision highp float;

layout(location = 0) in  vec3 inPosition;
layout(location = 1) in  vec3 inNormal;

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec4 out_normal;


layout( binding = 0) uniform UniformBufferObject
{
    // Scene Transform
    mat4 sceneTransform;
    vec4 sceneScale;

    // Projection matrix
    mat4 projectionMatrix;

    // Scene ambient color
    vec4  ambient_color;

    // Light Info
    vec4 lightLocation;
    vec4 lightIntensities;
} ubo;

layout( binding = 1) readonly buffer MaterialsSSBO
{
    vec4 colors[];
} ssbo;

void main()
{
    mat4 meshTransform =  ubo.sceneTransform * ubo.projectionMatrix;

    vec4 vpos = vec4(inPosition.xyz, 1.0) * meshTransform;
    vpos *=  ubo.sceneScale;

    gl_Position           = vpos/vpos.w;
    out_color             = ssbo.colors[gl_InstanceIndex].rgb;
    out_normal            = vec4(inNormal,1.0);
}
