#ifndef ASSET_TOOLS_H
#define ASSET_TOOLS_H

#include "Asset_Tools.h"
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
                                      VkAabbPositionsKHR desiredBounds, // for now, im going to assume the center of this box is always 0,0,0
                                      bool               maintainSceneAspectRatio)    
{
    glm::vec3 originalAabbSize = { originalAABB.maxX - originalAABB.minX,     // original AABB width
                                   originalAABB.maxY - originalAABB.minY,     // original AABB height
                                   originalAABB.maxZ - originalAABB.minZ };  // original AABB depth
    {
        assert (originalAabbSize.x == fabs (originalAabbSize.x));
        assert (originalAabbSize.y == fabs (originalAabbSize.y));
        assert (originalAabbSize.z == fabs (originalAabbSize.z));
    }

    glm::vec3 desiredBoundsSize = { desiredBounds.maxX - desiredBounds.minX,   // Desired bounds width
                                    desiredBounds.maxY - desiredBounds.minY,   // Desired bounds height
                                    desiredBounds.maxZ - desiredBounds.minZ }; // Desired bounds depth
    {// check size is positive
        assert (desiredBoundsSize.x == fabs (desiredBoundsSize.x));
        assert (desiredBoundsSize.y == fabs (desiredBoundsSize.y));
        assert (desiredBoundsSize.z == fabs (desiredBoundsSize.z));
    }
    float    largestGap              = 0.0f;
    uint32_t idxOfAxisWithLargestGap = UINT32_MAX;

    glm::vec3 boundsDifference = originalAabbSize - desiredBoundsSize;

    for (uint32_t i = 0; i < 3; i++)
    {
        if (boundsDifference[i] > largestGap)
        {
            largestGap              = boundsDifference[i];
            idxOfAxisWithLargestGap = i;
        }
    }
    
    glm::vec3 originalAABBCenter = { originalAABB.minX + (originalAabbSize.x / 2),
                                     originalAABB.minY + (originalAabbSize.y / 2),
                                     originalAABB.minZ + (originalAabbSize.z / 2) };

    glm::vec3 desiredBoundsCenter = { desiredBounds.minX + (desiredBoundsSize.x / 2),
                                      desiredBounds.minY + (desiredBoundsSize.y / 2),
                                      desiredBounds.minZ + (desiredBoundsSize.z / 2) };

    printf ("originalAABBCenter   = { %.4f, %.4f, %.4f }\n", originalAABBCenter.x, originalAABBCenter.y, originalAABBCenter.z);
    printf ("desiredBoundsCenter = { %.4f, %.4f, %.4f }\n", desiredBoundsCenter.x, desiredBoundsCenter.y, desiredBoundsCenter.z);

    // Translate by the amount that places the scene AABB center point at the origin
    glm::mat4 translate2OriginMatrix = glm::mat4 (1.0f, 0.0f, 0.0f, -(originalAABBCenter.x),
                                                  0.0f, 1.0f, 0.0f, -(originalAABBCenter.y),
                                                  0.0f, 0.0f, 1.0f, -(originalAABBCenter.z),
                                                  0.0f, 0.0f, 0.0f, 1.0f);

    glm::vec3 scaleAmt = glm::vec3 (1.0f, 1.0f, 1.0f);

    if (idxOfAxisWithLargestGap != UINT32_MAX)
    {
        float scale = desiredBoundsSize[idxOfAxisWithLargestGap] / originalAabbSize[idxOfAxisWithLargestGap];
        scaleAmt = glm::vec3 (scale, scale, scale);
    }
    printf ("scaleAmt = { %.4f, %.4f, %.4f }\n", scaleAmt.x, scaleAmt.y, scaleAmt.z);
    glm::mat4 scaleToDesiredBoundsSize = glm::mat4 (scaleAmt.x, 0.0f,       0.0f,       0.0,
                                                    0.0f,       scaleAmt.y, 0.0f,       0.0,
                                                    0.0f,       0.0f,       scaleAmt.z * 0.5, 0.0,
                                                    0.0f,       0.0f,       0.0f,       1.0f);

    glm::mat4 translate2DesiredAabbCenter = glm::mat4 (1.0f, 0.0f, 0.0f, (desiredBoundsCenter.x),
                                                       0.0f, 1.0f, 0.0f, (desiredBoundsCenter.y),
                                                       0.0f, 0.0f, 1.0f, (desiredBoundsCenter.z),
                                                       0.0f, 0.0f, 0.0f, 1.0f);

    return  translate2OriginMatrix * scaleToDesiredBoundsSize * translate2DesiredAabbCenter;




}
#endif