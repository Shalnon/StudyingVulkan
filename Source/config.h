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

#ifndef CONFIG_H_IN
#define CONFIG_H_IN


#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0

#define PROJECT_NAME    "StudyingVulkan"
#define SHADER_ROOT_DIR "C:/git/CmakeExperiments/StudyingVulkan/StudyingVulkan/Source/Shaders"
#define ASSET_DIR       "C:/git/CmakeExperiments/StudyingVulkan/StudyingVulkan/Assets"
#define DEBUG_PRINTS_ENABLE 0

static const char* pShaderRootDirectory = SHADER_ROOT_DIR;
static const char* pAssetDirectory      = ASSET_DIR;
 

#endif //CONFIG_H_IN
