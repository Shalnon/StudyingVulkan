//#include "Asset_Tools.h"
//#include <assert.h> 

/*********************************************************************************************/
/*=====================================Model loading=========================================*/
/*********************************************************************************************/



//void Copy3dModelAssetFromFile (const char* pFilePath, MeshGeometryData** pMeshesOut)
//{
//    const aiScene* pScene = aiImportFile (pFilePath, aiProcessPreset_TargetRealtime_MaxQuality);
//
//    if (pScene->HasMeshes () == true)
//    {
//        uint32_t pathLength  = strlen (pFilePath);
//        char*    pIndentation = static_cast<char*>(calloc(pathLength + 14, sizeof(char)));
//
//        printf ("3d model file %s: contains %u meshes\n", pFilePath, pScene->mNumMeshes);
//        printf ("%s: contains %u materials\n", pIndentation, pScene->mNumMaterials);
//
//        MeshGeometryData* pMeshesOut = static_cast<MeshGeometryData*>(calloc (pScene->mNumMeshes, sizeof (MeshGeometryData)));
//
//        // This assert makes sure that floats are stored contiguously
//        assert ((sizeof (aiVector3D) / 3 == sizeof (float))  && (sizeof(ai_real) == sizeof(float)));
//
//        const uint32_t floatsPerPosition = sizeof(aiVector3D) / sizeof(float);  /*|*/ assert(floatsPerPosition == 3);
//        const uint32_t floatsPerNormal   = sizeof(aiVector3D) / sizeof(float);  /*|*/ assert(floatsPerNormal   == 3);
//        const uint32_t floatsPerColor    = sizeof(aiColor4D)  / sizeof(float);  /*|*/ assert(floatsPerColor    == 4);
//
//        const uint32_t indicesPerTri = 3;
//
//        for (uint32_t meshIdx = 0; meshIdx < pScene->mNumMeshes; meshIdx++)
//        {
//            const aiMesh*     pAiMeshIn = pScene->mMeshes[meshIdx];
//            MeshGeometryData* pMeshOut  = &(pMeshesOut[meshIdx]);
//               
//            assert(pAiMeshIn->HasPositions() == true);
//            assert(pAiMeshIn->HasNormals()   == true);
//            assert(pAiMeshIn->mNumVertices    > 0);
//
//            //pAiMeshIn->mAABB
//            const uint32_t numVertices  = pAiMeshIn->mNumVertices;
//            const uint32_t numTriangles = pAiMeshIn->mNumFaces;
//
//            pMeshOut->numVertices  = numVertices;
//            pMeshOut->numTriangles = numTriangles;
//
//            pMeshOut->pVertexPositions = static_cast<float*   >(calloc(numVertices  * floatsPerPosition,  sizeof(float)   ));
//            pMeshOut->pVertexNormals   = static_cast<float*   >(calloc(numVertices  * floatsPerNormal  ,  sizeof(float)   ));
//            pMeshOut->pTriangleIndices = static_cast<uint32_t*>(calloc(numTriangles * indicesPerTri    ,  sizeof(uint32_t)));
//
//            // Copy position and normal data from ai mesh
//            memcpy (pMeshOut->pVertexPositions, pAiMeshIn->mVertices, sizeof(float) * numVertices * floatsPerPosition);
//            memcpy (pMeshOut->pVertexNormals  , pAiMeshIn->mNormals , sizeof(float) * numVertices * floatsPerNormal);
//
//            // The face indices arent stored contiguously, so we need to loop over them.
//            uint32_t indicesInCount = 0;
//            for (uint32_t faceIdx = 0; faceIdx < pAiMeshIn->mNumFaces; faceIdx++)
//            {
//                const aiFace* pFace = &(pAiMeshIn->mFaces[faceIdx]);
//                assert (pFace->mNumIndices == 3);
//
//                pMeshOut->pTriangleIndices[indicesInCount++] = pFace->mIndices[0];
//                pMeshOut->pTriangleIndices[indicesInCount++] = pFace->mIndices[1];
//                pMeshOut->pTriangleIndices[indicesInCount++] = pFace->mIndices[2];
//
//            }
//            //const uint32_t assimpColorSetIdx = 0;
//            //if (pMesh->HasVertexColors(assimpColorSetIdx) == true)
//            //{
//            //    pColors = static_cast<float*>(malloc (pMesh->mNumVertices * floatsPerColor));
//            //    memcpy (pColors, pMesh->mColors, sizeof (float) * pMesh->mNumVertices * floatsPerColor);
//            //}
//        }
//
//    }
//}

