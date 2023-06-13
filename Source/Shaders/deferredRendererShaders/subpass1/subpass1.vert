//glslc --target-env=vulkan1.3 -fshader-stage=vert -o vert.spv subpass1.vert 

#version 420

precision highp float;

layout(location = 0) out vec4 in_positionOnFullScreenQuad;

vec3 fullScreenQuadVertices[6] = vec3[](
    vec3(-1.0, 1.0, 0.5),   // A
    vec3(-1.0, -1.0, 0.5),  // B
    vec3( 1.0, -1.0, 0.5),  // C
    vec3(-1.0, 1.0, 0.5),   // A
    vec3( 1.0, -1.0, 0.5),  // C
    vec3( 1.0, 1.0, 0.5)    // D
);


void main()
{
    in_positionOnFullScreenQuad = vec4(fullScreenQuadVertices[gl_VertexIndex], 1.0);
    gl_Position                 = vec4(fullScreenQuadVertices[gl_VertexIndex], 1.0);
}
