#ifndef ASSET_TOOLS_H
#define ASSET_TOOLS_H


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

#endif // ASSET_TOOLS_H