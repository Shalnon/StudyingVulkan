#ifndef ASSET_TOOLS_H
#define ASSET_TOOLS_H

#include "Asset_Tools.h"
#include <assert.h> 

/*********************************************************************************************/
/*=====================================Model loading=========================================*/
/*********************************************************************************************/


void Copy3dModelAssetFromFile (const char* pFilePath, MeshGeometryData** pMeshesOut)
{
    const aiScene* pScene = aiImportFile (pFilePath, aiProcessPreset_TargetRealtime_MaxQuality);

    if (pScene->HasMeshes () == true)
    {
        uint32_t pathLength  = strlen (pFilePath);
        char*    pIndentation = static_cast<char*>(calloc(pathLength + 14, sizeof(char)));

        printf ("3d model file %s: contains %u meshes\n", pFilePath, pScene->mNumMeshes);
        printf ("%s: contains %u materials\n", pIndentation, pScene->mNumMaterials);

        MeshGeometryData* pMeshesOut = static_cast<MeshGeometryData*>(calloc (pScene->mNumMeshes, sizeof (MeshGeometryData)));

        // This assert makes sure that floats are stored contiguously
        assert ((sizeof (aiVector3D) / 3 == sizeof (float))  && (sizeof(ai_real) == sizeof(float)));

        const uint32_t floatsPerPosition = sizeof(aiVector3D) / sizeof(float);  /**/ assert(floatsPerPosition == 3);
        const uint32_t floatsPerNormal   = sizeof(aiVector3D) / sizeof(float);  /**/ assert(floatsPerNormal   == 3);
        const uint32_t floatsPerColor    = sizeof(aiColor4D)  / sizeof(float);  /**/ assert(floatsPerColor    == 4);

        const uint32_t indicesPerTri = 3;

        for (uint32_t meshIdx = 0; meshIdx < pScene->mNumMeshes; meshIdx++)
        {
            const aiMesh*     pAiMeshIn = pScene->mMeshes[meshIdx];
            MeshGeometryData* pMeshOut  = &(pMeshesOut[meshIdx]);
               
            assert(pAiMeshIn->HasPositions() == true);
            assert(pAiMeshIn->HasNormals()   == true);
            assert(pAiMeshIn->mNumVertices    > 0);

            //pAiMeshIn->mAABB
            const uint32_t numVertices = pAiMeshIn->mNumVertices;
            const uint32_t numTriangles = pAiMeshIn->mNumFaces;

            pMeshOut->numVertices  = numVertices;
            pMeshOut->numTriangles = numTriangles;

            pMeshOut->pVertexPositions = static_cast<float*   >(calloc(numVertices  * floatsPerPosition,  sizeof(float)));
            pMeshOut->pVertexNormals   = static_cast<float*   >(calloc(numVertices  * floatsPerNormal  ,  sizeof (float)));
            pMeshOut->pTriangleIndices = static_cast<uint32_t*>(calloc(numTriangles * indicesPerTri    ,  sizeof(uint32_t)));

            // Copy position and normal data from ai mesh
            memcpy (pMeshOut->pVertexPositions, pAiMeshIn->mVertices, sizeof(float) * numVertices * floatsPerPosition);
            memcpy (pMeshOut->pVertexNormals  , pAiMeshIn->mNormals , sizeof(float) * numVertices * floatsPerNormal);

            // The face indices arent stored contiguously, so we need to loop over them.
            uint32_t indicesInCount = 0;
            for (uint32_t faceIdx = 0; faceIdx < pAiMeshIn->mNumFaces; faceIdx++)
            {
                const aiFace* pFace = &(pAiMeshIn->mFaces[faceIdx]);
                assert (pFace->mNumIndices == 3);

                pMeshOut->pTriangleIndices[indicesInCount++] = pFace->mIndices[0];
                pMeshOut->pTriangleIndices[indicesInCount++] = pFace->mIndices[1];
                pMeshOut->pTriangleIndices[indicesInCount++] = pFace->mIndices[2];

            }
            //const uint32_t assimpColorSetIdx = 0;
            //if (pMesh->HasVertexColors(assimpColorSetIdx) == true)
            //{
            //    pColors = static_cast<float*>(malloc (pMesh->mNumVertices * floatsPerColor));
            //    memcpy (pColors, pMesh->mColors, sizeof (float) * pMesh->mNumVertices * floatsPerColor);
            //}
        }

    }
}


glm::mat4 GetTransform_FitAABBToAABB (VkAabbPositionsKHR originalAABB,
                                      VkAabbPositionsKHR desiredBounds,
                                      bool               maintainSceneAspectRatio)
{
    glm::vec3 scaleVec         = glm::vec3 (1.0f, 1.0f, 1.0f);
    glm::vec3 translationVec   = glm::vec3 (0.0f, 0.0f, 0.0f);
    glm::vec3 originalAabbSize = { originalAABB.maxX - originalAABB.minX,     // original AABB width
                                   originalAABB.maxY - originalAABB.minY,     // original AABB height
                                   originalAABB.maxZ - originalAABB.minZ };  // original AABB depth


    glm::vec3 desiredBoundsSize = { desiredBounds.maxX - desiredBounds.minX,   // Desired bounds width
                                    desiredBounds.maxY - desiredBounds.minY,   // Desired bounds height
                                    desiredBounds.maxZ - desiredBounds.minZ }; // Desired bounds depth

    //@TODO: move this into a function so i can get the scale separately from a transform matrix
    if (maintainSceneAspectRatio == true)
    {
        // Find the axis with the largest gap in sizes between the original AABB and target AABB sizes
        glm::vec3 boundsDifferences  = desiredBoundsSize - originalAabbSize;

        float scale = 1.0f;

        for (uint32_t i = 0;  i < 3; i++)
        {
            if (originalAabbSize[i] != 0.0)
            {
                float scaleToDesiredSizeOnAxis = desiredBoundsSize[i] / originalAabbSize[i];
                if (scaleToDesiredSizeOnAxis < scale)
                {
                    scale = scaleToDesiredSizeOnAxis;
                }
            }
        }

        scaleVec = glm::vec3 (scale, scale, scale);
    }
    else
    {
        for (uint32_t i = 0; i < 3; i++)
        {
            scaleVec[i] = desiredBoundsSize[i] / originalAabbSize[i];
        }
    }

    glm::vec3 originalAABBCenter = { originalAABB.minX + (originalAabbSize.x / 2),
                                     originalAABB.minY + (originalAabbSize.y / 2),
                                     originalAABB.minZ + (originalAabbSize.z / 2) };

    glm::vec3 desiredBoundsCenter = { desiredBounds.minX + (desiredBoundsSize.x / 2),
                                      desiredBounds.minY + (desiredBoundsSize.y / 2),
                                      desiredBounds.minZ + (desiredBoundsSize.z / 2) };

    // Translate by the amount that places the scene AABB center point at the origin
    glm::mat4 translate2OriginMatrix = glm::mat4 (1.0f, 0.0f, 0.0f, -originalAABBCenter[0],
                                                  0.0f, 1.0f, 0.0f, -originalAABBCenter[1],
                                                  0.0f, 0.0f, 1.0f, -originalAABBCenter[2],
                                                  0.0f, 0.0f, 0.0f, 1.0f);

    // Scale relative to the origin
    glm::mat4 scaleAroundOriginMatrix = glm::mat4 (scaleVec.x, 0.0f,       0.0f,       0.0f,
                                                   0.0f,       scaleVec.y, 0.0f,       0.0f,
                                                   0.0f,       0.0f,       scaleVec.z, 0.0f,
                                                   0.0f,       0.0f,       0.0f,       1.0f);

    // Translate by the amount that places the scene AABB center point at the center point of desiredBounds
    glm::mat4 translateToDesiredLoc = glm::mat4 (1.0f, 0.0f, 0.0f, -desiredBoundsCenter[0],
                                                 0.0f, 1.0f, 0.0f, -desiredBoundsCenter[1],
                                                 0.0f, 0.0f, 1.0f, -desiredBoundsCenter[2],
                                                 0.0f, 0.0f, 0.0f, 1.0f);

    // Combine the transforms into one matrix
    glm::mat4 fitAabb2AabbMatrix = translate2OriginMatrix * scaleAroundOriginMatrix * translateToDesiredLoc;

    return fitAabb2AabbMatrix;
}

#endif