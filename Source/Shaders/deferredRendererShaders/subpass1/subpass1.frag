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

//compile command: glslc --target-env=vulkan1.3 -fshader-stage=frag -o frag.spv subpass1.frag 

#version 420
//@note: implicitely using GL_KHR_vulkan_glsl

precision highp float;


layout( binding = 0 ) uniform UniformBufferObject
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

layout (input_attachment_index=0, set=0, binding=1 ) uniform subpassInput diffuseColorInputAttachment;
layout (input_attachment_index=1, set=0, binding=2 ) uniform subpassInput normalVectorInputAttachment;
layout (input_attachment_index=2, set=0, binding=3 ) uniform subpassInput worldPositionInputAttachment;


layout(location = 0) out vec3 fragColor;


void main()
{
    vec4 diffuseColor         = subpassLoad(diffuseColorInputAttachment);
    vec4 surfaceNormal        = subpassLoad(normalVectorInputAttachment);
    vec4 surfaceWorldPosition = subpassLoad(worldPositionInputAttachment);

    // gl_FragCoord.z value from subpass 0 stored in the alpha component.
    float depthValue        = surfaceWorldPosition.w;
    float distanceFromLight = distance(ubo.lightLocation.xyz,  surfaceWorldPosition.xyz);
	vec3  surfaceToLight    = normalize(ubo.lightLocation.xyz - surfaceWorldPosition.xyz);
	float cosaoi            = dot(surfaceNormal.xyz, surfaceToLight);

	float Diffuse_Coefficient = max(0.0,cosaoi);

	vec3 Diffuse = diffuseColor.rgb * ubo.lightIntensities.xyz * Diffuse_Coefficient;

	fragColor = Diffuse + (ubo.ambient_color.rgb * depthValue);
}
