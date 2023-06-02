#version 420

//glslc --target-env=vulkan1.3 -fshader-stage=frag -o vert.spv triangle.frag

precision mediump float;

layout(location = 0) in vec3 in_color;

layout(location = 0) out vec4 out_color;

void main()
{
	out_color = vec4(in_color, 1.0);
}
