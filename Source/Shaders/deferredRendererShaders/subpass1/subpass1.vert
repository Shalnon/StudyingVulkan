/* Copyright 2023 Sean Halnon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**  http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissionsand
**  limitations under the License.
**/

//compile command: glslc --target-env=vulkan1.3 -fshader-stage=vert -o vert.spv subpass1.vert 

#version 420
//implicitly using GL_KHR_vulkan_glsl

precision highp float;


vec3 fullScreenQuadVertices[6] = vec3[](
    vec3(-1.0, 1.0, 0.5),   // A             B________C
    vec3(-1.0, -1.0, 0.5),  // B             |       /|
    vec3( 1.0, -1.0, 0.5),  // C             |     /  |
    vec3(-1.0, 1.0, 0.5),   // A             |   /    |
    vec3( 1.0, -1.0, 0.5),  // C             | /      |
    vec3( 1.0, 1.0, 0.5)    // D             A--------D
);


void main()
{
    gl_Position = vec4(fullScreenQuadVertices[gl_VertexIndex], 1.0);
}
