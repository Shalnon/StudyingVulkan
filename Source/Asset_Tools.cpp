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
#ifndef MY_ASSET_TOOLS_CPP
#define MY_ASSET_TOOLS_CPP

#include "Asset_Tools.h"
#include "StudyingVulkan.h"
#include <assert.h> 
#include <glm/gtx/transform.hpp>

/*********************************************************************************************/
/*=====================================Model loading=========================================*/
/*********************************************************************************************/

void PrintGeometryInformation (const aiScene* pScene)
{
    printf ("-----------Printing Scene Information------------");
    printf ("\tnumMeshes   = %u\n", pScene->mNumMeshes);
        //pScene->mMetaData->mNumProperties

    for (uint32_t meshIdx = 0; meshIdx < pScene->mNumMeshes; meshIdx++)
    {
        const aiMesh* pMesh = pScene->mMeshes[meshIdx];

        // Print name, or "#NULL#" if there isnt one.
        const char* pMeshName = pMesh->mName.C_Str ();
        printf ("--- mMeshes[%u]\t%s ---\n", meshIdx, (pMeshName == nullptr) ? "#NULL#" : pMeshName);

        uint32_t numFaces     = pMesh->mNumFaces;
        uint32_t numVertices  = pMesh->mNumVertices;
        uint32_t numTriangles = 0;
        uint32_t numQuads     = 0;
        uint32_t numNGons     = 0;
        uint32_t numEdges     = 0;

        for (uint32_t faceIdx = 0; faceIdx < numFaces; faceIdx++)
        {
            const aiFace* pFace = &(pMesh->mFaces[faceIdx]);
            if (pFace->mNumIndices == 3)
            {
                numTriangles++;
            }
            else if (pFace->mNumIndices == 4)
            {
                numQuads++;
            }
            else if (pFace->mNumIndices > 4)
            {
                numNGons++;
            }
            else if (pFace->mNumIndices == 2)
            {
                numEdges++;
            }
            else
            {
                printf ("Invalid number of face indices provided for face[%u]\n", faceIdx);
            }
        }

        printf ("\tnumFaces      = %u\n", numFaces);
        printf ("\tnumVertices   = %u\n", numVertices);
        printf ("\tnumTriangles  = %u\n", numTriangles);
        printf ("\tnumQuads      = %u\n", numQuads);
        printf ("\tnumNGons      = %u\n", numNGons);
        printf ("\tnumEdges      = %u\n", numEdges);

    }
}

glm::mat4 GetTransform_FitAABBToAABB (VkAabbPositionsKHR originalAABB,
                                      VkAabbPositionsKHR desiredBounds, // for now, im going to assume the center of this box is always 0,0,0
                                      bool               maintainSceneAspectRatio)    
{
    glm::vec3 originalAabbSize = { originalAABB.maxX - originalAABB.minX,    // original AABB width
                                   originalAABB.maxY - originalAABB.minY,    // original AABB height
                                   originalAABB.maxZ - originalAABB.minZ };  // original AABB depth

    glm::vec3 desiredBoundsSize = { desiredBounds.maxX - desiredBounds.minX,   // Desired bounds width
                                    desiredBounds.maxY - desiredBounds.minY,   // Desired bounds height
                                    desiredBounds.maxZ - desiredBounds.minZ }; // Desired bounds depth
#ifdef DEBUG // check size is positive
        assert (desiredBoundsSize.x == fabs (desiredBoundsSize.x));
        assert (desiredBoundsSize.y == fabs (desiredBoundsSize.y));
        assert (desiredBoundsSize.z == fabs (desiredBoundsSize.z));
        assert (originalAabbSize.x  == fabs (originalAabbSize.x) );
        assert (originalAabbSize.y  == fabs (originalAabbSize.y) );
        assert (originalAabbSize.z  == fabs (originalAabbSize.z) );
#endif

    glm::vec3 scaleAmt      = glm::vec3 (1.0f, 1.0f, 1.0f);
    glm::vec3 boundsDivided = desiredBoundsSize / originalAabbSize;
    bool      scaleDown     = true;

    if ((desiredBoundsSize.x > originalAabbSize.x) &&
        (desiredBoundsSize.y > originalAabbSize.y) &&
        (desiredBoundsSize.z > originalAabbSize.z))
    {
        scaleDown = false;
    }

    // if all bounds on target aabb are larger, than i need to scale up
    if (maintainSceneAspectRatio == true)
    {
        float scaleFactor = 1.0f;

        if (scaleDown == true)
        {// Find the axis on which the difference in AABB sizes is the SMALLEST

            scaleFactor =  boundsDivided.x;
            scaleFactor = (boundsDivided.y < scaleFactor) ? boundsDivided.y : scaleFactor;
            scaleFactor = (boundsDivided.z < scaleFactor) ? boundsDivided.z : scaleFactor;

            //for (uint32_t i = 0; i < 3; i++)
            //{
            //    if (boundsDivided[i] < scaleFactor)
            //    {
            //        scaleFactor = boundsDivided[i];
            //    }
            //}
        }
        else
        { // Find the axis on which the difference in AABB sizes is the LARGEST
            for (uint32_t i = 0; i < 3; i++)
            {
                if (boundsDivided[i] > scaleFactor)
                {
                    scaleFactor = boundsDivided[i];
                }
            }
        }


        scaleAmt = glm::vec3 (scaleFactor, scaleFactor, scaleFactor);
    }
    else
    {
        scaleAmt = boundsDivided;
    }
    

    glm::mat4 scaleToDesiredBoundsSize = glm::mat4 (scaleAmt.x, 0.0f,       0.0f,       0.0f,
                                                    0.0f,       scaleAmt.y, 0.0f,       0.0f,
                                                    0.0f,       0.0f,       scaleAmt.z, 0.0f,
                                                    0.0f,       0.0f,       0.0f,       1.0f);

    glm::vec3 originalAABBCenter  = { originalAABB.minX + (originalAabbSize.x / 2),
                                      originalAABB.minY + (originalAabbSize.y / 2),
                                      originalAABB.minZ + (originalAabbSize.z / 2) };

    glm::vec3 desiredBoundsCenter = { desiredBounds.minX + (desiredBoundsSize.x / 2),
                                      desiredBounds.minY + (desiredBoundsSize.y / 2),
                                      desiredBounds.minZ + (desiredBoundsSize.z / 2) };

#ifdef DEBUG
    printf ("scaleAmt = { %.4f, %.4f, %.4f }\n", scaleAmt.x, scaleAmt.y, scaleAmt.z);
    printf ("originalAABBCenter   = { %.4f, %.4f, %.4f }\n", originalAABBCenter.x, originalAABBCenter.y, originalAABBCenter.z);
    printf ("desiredBoundsCenter  = { %.4f, %.4f, %.4f }\n", desiredBoundsCenter.x, desiredBoundsCenter.y, desiredBoundsCenter.z);
#endif
    // Translate by the amount that places the scene AABB center point at the origin
    glm::mat4 translate2OriginMatrix = glm::mat4 (1.0f, 0.0f, 0.0f, -(originalAABBCenter.x),
                                                  0.0f, 1.0f, 0.0f, -(originalAABBCenter.y),
                                                  0.0f, 0.0f, 1.0f, -(originalAABBCenter.z),
                                                  0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 translate2DesiredAabbCenter = glm::mat4 (1.0f, 0.0f, 0.0f, (desiredBoundsCenter.x),
                                                       0.0f, 1.0f, 0.0f, (desiredBoundsCenter.y),
                                                       0.0f, 0.0f, 1.0f, (desiredBoundsCenter.z),
                                                       0.0f, 0.0f, 0.0f, 1.0f);


    // Create rotation matrix
    glm::mat4 identityMatrix = glm::mat4 (1.0f, 0.0f, 0.0f, 0.0f,
                                          0.0f, 1.0f, 0.0f, 0.0f,
                                          0.0f, 0.0f, 1.0f, 0.0f,
                                          0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 rotationMatrix = glm::rotate (identityMatrix, 45.0f, glm::vec3 (0.0f, 1.0f, 0.0f));


    return  translate2OriginMatrix * rotationMatrix * scaleToDesiredBoundsSize * translate2DesiredAabbCenter;
}

glm::mat4 GetSceneTransform (VkAabbPositionsKHR sceneBounds,
                             VkAabbPositionsKHR meshDataAabb,
                             glm::vec3          sceneRotation,
                             glm::mat4*         pNormalRotationOut)
{
    const bool maintainMeshAspectRatio      = true;
    glm::vec3  scaleAmt                     = ScaleAabbToAabb (meshDataAabb, sceneBounds, maintainMeshAspectRatio);
    glm::vec3  meshDataAabbSize             = GetAabbSize (meshDataAabb);
    glm::vec3  desiredSceneBoundsDimensions = GetAabbSize (sceneBounds);

    glm::vec3 meshAabbCenter = { meshDataAabb.minX + (meshDataAabbSize.x / 2),
                                 meshDataAabb.minY + (meshDataAabbSize.y / 2),
                                 meshDataAabb.minZ + (meshDataAabbSize.z / 2) };

    glm::vec3 desiredBoundsCenter = { sceneBounds.minX + (desiredSceneBoundsDimensions.x / 2),
                                      sceneBounds.minY + (desiredSceneBoundsDimensions.y / 2),
                                      sceneBounds.minZ + (desiredSceneBoundsDimensions.z / 2) };


    glm::mat4 scaleToDesiredBoundsSize = glm::mat4 (scaleAmt.x, 0.0f,       0.0f,       0.0f,
                                                    0.0f,       scaleAmt.y, 0.0f,       0.0f,
                                                    0.0f,       0.0f,       scaleAmt.z, 0.0f,
                                                    0.0f,       0.0f,       0.0f,       1.0f);

#ifdef DEBUG
    printf ("scaleAmt = { %.4f, %.4f, %.4f }\n", scaleAmt.x, scaleAmt.y, scaleAmt.z);
    printf ("originalAABBCenter   = { %.4f, %.4f, %.4f }\n", meshAabbCenter.x, meshAabbCenter.y, meshAabbCenter.z);
    printf ("desiredBoundsCenter  = { %.4f, %.4f, %.4f }\n", desiredBoundsCenter.x, desiredBoundsCenter.y, desiredSceneBoundsDimensions.z);
#endif
    // Translate by the amount that places the center of the scene AABB at origin
    glm::mat4 translate2OriginMatrix = glm::mat4 (1.0f, 0.0f, 0.0f, -(meshAabbCenter.x),
                                                  0.0f, 1.0f, 0.0f, -(meshAabbCenter.y),
                                                  0.0f, 0.0f, 1.0f, -(meshAabbCenter.z),
                                                  0.0f, 0.0f, 0.0f, 1.0f);

    glm::mat4 translate2DesiredAabbCenter = glm::mat4 (1.0f, 0.0f, 0.0f, desiredBoundsCenter.x,
                                                       0.0f, 1.0f, 0.0f, desiredBoundsCenter.y,
                                                       0.0f, 0.0f, 1.0f, desiredBoundsCenter.z,
                                                       0.0f, 0.0f, 0.0f, 1.0f);

    // Initialize the rotation matrix with an identity matrix, than update if scene rotation is non-zero
    glm::mat4 rotationMatrix = glm::identity<glm::mat4>();
    
    if (sceneRotation.x != 0)
    {
        rotationMatrix = glm::rotate (rotationMatrix, sceneRotation.x, glm::vec3 (1.0f, 0.0f, 0.0f));
    }
    if (sceneRotation.y != 0)
    {
        rotationMatrix = glm::rotate (rotationMatrix, sceneRotation.y, glm::vec3 (0.0f, 1.0f, 0.0f));
    }
    if (sceneRotation.z != 0)
    {
        rotationMatrix = glm::rotate (rotationMatrix, sceneRotation.z, glm::vec3 (0.0f, 0.0f, 1.0f));
    }

    *pNormalRotationOut = rotationMatrix;

    return  translate2OriginMatrix * rotationMatrix * scaleToDesiredBoundsSize * translate2DesiredAabbCenter;
}
#endif // MY_ASSET_TOOLS_CPP