#version 420




 //glslc --target-env=vulkan1.3 -fshader-stage=vert -o vert.spv triangle.vert 
precision highp float;

layout(location = 0) in  vec3 inPosition;
layout(location = 0) out vec3 out_color;

vec3 triangle_colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main()
{
    gl_Position = vec4(inPosition.xyz, 1.0);

    out_color = triangle_colors[gl_VertexIndex%3];
}
