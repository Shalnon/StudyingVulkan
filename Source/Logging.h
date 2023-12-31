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

#ifndef LOGGING_H
#define LOGGING_H

#include <glm\glm.hpp>

inline void PrintNamedMatrix (const char* pName, glm::mat4* pMatrix)
{
    printf ("Name: %s\n", pName);
    printf ("Matrix - [%.4f, %.4f, %.4f, %.4f]\n", (*pMatrix)[0][0], (*pMatrix)[0][1], (*pMatrix)[0][2], (*pMatrix)[0][3] );
    printf ("         [%.4f, %.4f, %.4f, %.4f]\n", (*pMatrix)[1][0], (*pMatrix)[1][1], (*pMatrix)[1][2], (*pMatrix)[1][3] );
    printf ("         [%.4f, %.4f, %.4f, %.4f]\n", (*pMatrix)[2][0], (*pMatrix)[2][1], (*pMatrix)[2][2], (*pMatrix)[2][3] );
    printf ("         [%.4f, %.4f, %.4f, %.4f]\n", (*pMatrix)[3][0], (*pMatrix)[3][1], (*pMatrix)[3][2], (*pMatrix)[3][3] );
}

inline void PrintNamedVec3 (const char* pName, glm::vec3 vector)
{
    printf ("Vec3 %s = { %.4f, %.4f, %.4f }", pName, vector.x, vector.y, vector.z);
}

inline void PrintNamedVec4 (const char* pName, glm::vec4 vector)
{
    printf ("Vec4 %s = { %.4f, %.4f, %.4f, %.4f }\n", pName, vector.x, vector.y, vector.z, vector.w);
}

#endif // LOGGING_H