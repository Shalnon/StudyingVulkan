#version 420
/* Copyright (c) 2019, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 //glslc --target-env=vulkan1.3 -fshader-stage=vert -o vert.spv triangle.vert 
precision mediump float;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 out_color;

//vec2 triangle_positions[3] = vec2[](
//    vec2(0.5, -0.5),
//    vec2(0.5, 0.5),
//    vec2(-0.5, 0.5)
//);

vec3 triangle_colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main()
{
    gl_Position = vec4(inPosition.xy, 0.0, 1.0);

    out_color = triangle_colors[gl_VertexIndex%3];
}
