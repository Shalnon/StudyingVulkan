//glslc --target-env=vulkan1.3 -fshader-stage=vert -o vert.spv triangle.vert 

#version 420
precision highp float;

layout(binding = 0) uniform UniformBufferObject
{
    mat4 modelMatrix[1];
    vec4 sceneScale;
} ubo;

layout(location = 0) in  vec3 inPosition;
layout(location = 0) out vec3 out_color;

vec3 triangle_colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main()
{
    mat4 meshTransform = ubo.modelMatrix[gl_InstanceIndex];

    gl_Position = vec4(inPosition.xy, 0.0, 1.0) * meshTransform;

    out_color = triangle_colors[gl_VertexIndex%3];
}
