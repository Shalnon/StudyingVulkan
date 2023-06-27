#ifndef MY_ASSET_TOOLS_H
#define MY_ASSET_TOOLS_H


#include "volk.h"
#include <assimp\cimport.h>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <glm\glm.hpp>

#define MY_ASSIMP_PREPROCESSING_FLAGS     aiProcess_CalcTangentSpace              |  \
                                          aiProcess_GenSmoothNormals              |  \
                                          aiProcess_JoinIdenticalVertices         |  \
                                          aiProcess_ImproveCacheLocality          |  \
                                          aiProcess_RemoveRedundantMaterials      |  \
                                          aiProcess_SplitLargeMeshes              |  \
                                          aiProcess_Triangulate                   |  \
                                          aiProcess_GenUVCoords                   |  \
                                          aiProcess_SortByPType                   |  \
                                          aiProcess_FindDegenerates               |  \
                                          aiProcess_FindInvalidData
                                          


struct MeshGeometryData
{
    uint32_t  numTriangles;
    uint32_t* pTriangleIndices;

    uint32_t  numVertices;
    float*    pVertexPositions;
    float*    pVertexNormals;

    glm::mat4x4 modelMatrix;
    glm::vec3   center;
};

void PrintGeometryInformation (const aiScene* pScene);

glm::mat4 GetTransform_FitAABBToAABB (VkAabbPositionsKHR originalAABB,
                                      VkAabbPositionsKHR desiredBounds,
                                      bool               maintainSceneAspectRatio);

void Copy3dModelAssetFromFile (const char*        pFilePath, 
                               MeshGeometryData** pMeshesOut);

inline VkAabbPositionsKHR TranslateAabb (VkAabbPositionsKHR* pAABB, glm::vec3 translation)
{
    VkAabbPositionsKHR aabbOut =
    {
        /*...float....minX....*/ pAABB->minX + translation.x,
        /*...float....minY....*/ pAABB->maxX + translation.x,
        /*...float....minZ....*/ pAABB->minY + translation.y,
        /*...float....maxX....*/ pAABB->maxY + translation.y,
        /*...float....maxY....*/ pAABB->minZ + translation.z,
        /*...float....maxZ....*/ pAABB->maxZ + translation.z
    };

    return aabbOut;
}



inline glm::vec3 GetAabbSize (VkAabbPositionsKHR aabb)
{
    return glm::vec3 (aabb.maxX - aabb.minX,   // original AABB width
                      aabb.maxY - aabb.minY,   // original AABB height
                      aabb.maxZ - aabb.minZ);  // original AABB depth
}

inline glm::vec3 ScaleAabbToAabb (VkAabbPositionsKHR originalAABB,
                                  VkAabbPositionsKHR desiredBounds, // for now, im going to assume the center of this box is always 0,0,0
                                  bool               maintainSceneAspectRatio)
{
    glm::vec3 originalAabbSize  = GetAabbSize (originalAABB);
    glm::vec3 desiredBoundsSize = GetAabbSize (desiredBounds);
#ifdef DEBUG // check size is positive
    assert (desiredBoundsSize.x == fabs (desiredBoundsSize.x));
    assert (desiredBoundsSize.y == fabs (desiredBoundsSize.y));
    assert (desiredBoundsSize.z == fabs (desiredBoundsSize.z));
    assert (originalAabbSize.x == fabs (originalAabbSize.x));
    assert (originalAabbSize.y == fabs (originalAabbSize.y));
    assert (originalAabbSize.z == fabs (originalAabbSize.z));
#endif

    glm::vec3 scaleAmt = glm::vec3 (1.0f, 1.0f, 1.0f);
    glm::vec3 boundsDivided = desiredBoundsSize / originalAabbSize;
    bool      scaleDown = true;

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

            for (uint32_t i = 0; i < 3; i++)
            {
                if (boundsDivided[i] < scaleFactor)
                {
                    scaleFactor = boundsDivided[i];
                }
            }
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

   // printf ("scaleAmt = { %.4f, %.4f, %.4f }\n", scaleAmt.x, scaleAmt.y, scaleAmt.z);
   //
   // glm::mat4 scaleToDesiredBoundsSize = glm::mat4 (scaleAmt.x, 0.0f, 0.0f, 0.0f,
   //                                                 0.0f, scaleAmt.y, 0.0f, 0.0f,
   //                                                 0.0f, 0.0f, scaleAmt.z, 0.0f,
   //                                                 0.0f, 0.0f, 0.0f, 1.0f);

    return scaleAmt;
}

glm::mat4 GetSceneTransform (VkAabbPositionsKHR sceneBounds,
                                    VkAabbPositionsKHR meshDataAabb,
                                    glm::vec3          sceneRotation);

#endif // MY_ASSET_TOOLS_H