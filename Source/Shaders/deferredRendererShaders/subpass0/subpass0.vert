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
*/

//compile command: glslc --target-env=vulkan1.3 -fshader-stage=vert -o vert.spv subpass0.vert 

//@note: implicitely using GL_KHR_vulkan_glsl

#version 450
precision highp float;

layout(location = 0) in  vec3 inPosition;
layout(location = 1) in  vec3 inNormal;

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec4 out_normal;
layout(location = 2) out vec4 out_worldPosition;


layout( binding = 0) uniform UniformBufferObject
{
    // Scene Transform
    mat4 sceneTransform;
    vec4 sceneScale;

    // Projection matrix
    mat4 projectionMatrix;
    mat4 normalRotation;

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
    vec4 worldPosition = vec4(inPosition.xyz, 1.0) * ubo.sceneTransform;
    worldPosition *=  ubo.sceneScale;

    vec4 projectedVertexPosition = worldPosition * ubo.projectionMatrix;

    // Store frag depth in w component of world position attachment since it should always be 1, meaning overwriting w will not cause any transform data to be lose
    worldPosition.w = projectedVertexPosition.z / projectedVertexPosition.w;

    gl_Position           = projectedVertexPosition;
    out_color             = ssbo.colors[gl_InstanceIndex].rgb;
    out_normal            = vec4(inNormal,1.0) * ubo.normalRotation; 
    out_worldPosition     = worldPosition;
}
