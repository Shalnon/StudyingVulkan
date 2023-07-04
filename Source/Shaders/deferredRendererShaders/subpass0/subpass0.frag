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

//compile command: glslc --target-env=vulkan1.3 -fshader-stage=frag -o frag.spv subpass0.frag 

#version 420

precision highp float;

layout(location = 0) in vec3 in_color;
layout(location = 1) in vec4 in_normal;
layout(location = 2) in vec4 in_worldPosition;

layout(location = 0)      out vec3 out_color;
layout(location = 1)      out vec4 out_normal;
layout(location = 2)      out vec4 out_worldPosition;


void main()
{
    out_color         = in_color;
    out_normal        = in_normal;


    // ensures 1 is 1.0 without losing position data
    vec4 worldPositionValueOut = in_worldPosition/in_worldPosition.w;
    worldPositionValueOut.w    = gl_FragCoord.z;

    out_worldPosition = worldPositionValueOut;
}
