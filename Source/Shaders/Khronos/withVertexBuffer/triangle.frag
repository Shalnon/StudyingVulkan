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


//compile command: glslc --target-env=vulkan1.3 -fshader-stage=frag -o frag.spv triangle.frag 

#version 420
//@note: implicitely using GL_KHR_vulkan_glsl

precision highp float;

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_positionOnSurface;

layout(location = 0) out vec3 fragColor;



layout( binding = 0 ) uniform UniformBufferObject
{
    // Scene Transform
    mat4 sceneTransform;
    vec4 sceneScale;

    // Scene ambient color
    vec4  ambient_color;

    // Light Info
    vec4 lightLocation;
    vec4 lightIntensities;
} ubo;



void main()
{
    float distanceFromLight = distance(ubo.lightLocation.xyz,  in_positionOnSurface);
	vec3  surfaceToLight    = normalize(ubo.lightLocation.xyz- in_positionOnSurface);
	float cosaoi            = dot(in_normal.xyz, surfaceToLight);

	float Diffuse_Coefficient = max(0.0,cosaoi);

	vec3 Diffuse = in_color * ubo.lightIntensities.xyz * Diffuse_Coefficient;

	fragColor = Diffuse + ubo.ambient_color.rgb;
}
