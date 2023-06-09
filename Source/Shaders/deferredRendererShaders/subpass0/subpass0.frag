//glslc --target-env=vulkan1.3 -fshader-stage=frag -o frag.spv subpass0.frag 

#version 420

precision highp float;

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec4 in_normal;
layout(location = 2) in vec3 in_positionOnSurface;

layout(location = 0)      out vec3 out_color;
layout(location = 1)      out vec4 out_normal;


void main()
{
    out_color  = in_color;
    out_normal = in_normal;
}
