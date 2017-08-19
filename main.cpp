#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <json/json.hpp>

#include <iostream>
#include <string>

using json = nlohmann::json;

void to_json(json& j, const aiMatrix4x4& matrix)
{
    j = json::array({
            matrix.a1, matrix.a2, matrix.a3, matrix.a4,
            matrix.b1, matrix.b2, matrix.b3, matrix.b4,
            matrix.c1, matrix.c2, matrix.c3, matrix.c4,
            matrix.d1, matrix.d2, matrix.d3, matrix.d4,
        });
}

void to_json(json& j, aiVertexWeight weight)
{
    j = json {
        {"id", weight.mVertexId},
        {"weight", weight.mWeight}
    };
}

void to_json(json& j, const aiBone* pBone)
{
    j = json {
        {"name", pBone->mName.C_Str()},
        {"num_weights", pBone->mNumWeights},
        {"offset_matrix", pBone->mOffsetMatrix},
        {"weights", json::array()}
    };

    for (int i = 0; i < pBone->mNumWeights; ++i)
    {
        j["weights"].push_back(pBone->mWeights[i]);
    }
}

void to_json(json& j, const aiVector3D& vertex)
{
    j = json {vertex.x, vertex.y, vertex.z};
}

void to_json(json& j, const aiColor4D& color)
{
    j = json {color.r, color.g, color.b, color.a};
}

void to_json(json& j, const aiFace& face)
{
    unsigned int numIndices = face.mNumIndices;
    std::vector<unsigned int> indices(numIndices);

    for (unsigned int j = 0; j < numIndices; ++j)
    {
        indices[j] = face.mIndices[j];
    }

    j = json { {"num_indices", numIndices}, {"indices", indices} };
}

void to_json(json& j, const aiMesh* pMesh)
{
    j["name"] = pMesh->mName.C_Str();
    j["primitive_types"] = pMesh->mPrimitiveTypes;
    j["material_index"] = pMesh->mMaterialIndex;

    if (pMesh->HasPositions())
    {
        for (unsigned int i = 0; i < pMesh->mNumVertices; ++i)
        {
            j["vertices"].push_back(pMesh->mVertices[i]);
        }
    }

    if (pMesh->HasNormals())
    {
        for (unsigned int i = 0; i < pMesh->mNumVertices; ++i)
        {
            j["normals"].push_back(pMesh->mNormals[i]);
        }
    }

    if (pMesh->HasTangentsAndBitangents())
    {
        for (unsigned int i = 0; i < pMesh->mNumVertices; ++i)
        {
            j["tangents"].push_back(pMesh->mTangents[i]);
            j["bitangents"].push_back(pMesh->mBitangents[i]);
        }
    }

    if (pMesh->HasFaces())
    {
        for (unsigned int i = 0; i < pMesh->mNumVertices; ++i)
        {
            j["faces"].push_back(pMesh->mFaces[i]);
        }
    }

    if (pMesh->HasBones())
    {
        for (unsigned int i = 0; i < pMesh->mNumBones; ++i)
        {
            aiBone* pBone = pMesh->mBones[i];
            j["bones"].push_back(pBone);
        }
    }

    for (unsigned int i = 0; i < pMesh->GetNumColorChannels(); ++i)
    {
        if (pMesh->HasVertexColors(i))
        {
            std::string key = std::to_string(i);
            std::vector<aiColor4D> colors(pMesh->mNumVertices);
            for (unsigned int j = 0; j < pMesh->mNumVertices; ++j)
            {
                colors[j] = (pMesh->mColors[i][j]);
            }

            j["colors"][key] = colors;
        }
    }

    for (unsigned int i = 0; i < pMesh->GetNumUVChannels(); ++i)
    {
        if (pMesh->HasTextureCoords(i))
        {
            std::string key = std::to_string(i);
            unsigned int size = pMesh->mNumUVComponents[i];

            std::vector<aiVector3D> uvs(pMesh->mNumVertices);
            for (unsigned int j = 0; j < pMesh->mNumVertices; ++j)
            {
                uvs[j] = (pMesh->mTextureCoords[i][j]);
            }

            j["texturecoords"][key] = {
                {"numcomponents", size},
                {"uvs", uvs}
            };
        }
    }
}

void to_json(json& j, const aiMaterial* pMaterial) {}
void to_json(json& j, const aiTexture* pTexture) {}
void to_json(json& j, const aiLight* pLight) {}
void to_json(json& j, const aiCamera* pCamera) {}
void to_json(json& j, const aiAnimation* pAnimation) {}
void to_json(json& j, const aiNode* pNode) {}

void to_json(json& j, const aiScene* pScene)
{
    j["flags"] = pScene->mFlags;

    j["num_meshes"] = pScene->mNumMeshes;
    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
    {
        j["meshes"].push_back(pScene->mMeshes[i]);
    }

    j["num_materials"] = pScene->mNumMaterials;
    for (unsigned int i = 0; i < pScene->mNumMaterials; ++i)
    {
        j["materials"].push_back(pScene->mMaterials[i]);
    }

    j["num_textures"] = pScene->mNumTextures;
    for (unsigned int i = 0; i < pScene->mNumTextures; ++i)
    {
        j["textures"].push_back(pScene->mTextures[i]);
    }

    j["num_lights"] = pScene->mNumLights;
    for (unsigned int i = 0; i < pScene->mNumLights; ++i)
    {
        j["lights"].push_back(pScene->mLights[i]);
    }

    j["num_cameras"] = pScene->mNumCameras;
    for (unsigned int i = 0; i < pScene->mNumCameras; ++i)
    {
        j["cameras"].push_back(pScene->mCameras[i]);
    }

    j["num_animations"] = pScene->mNumAnimations;
    for (unsigned int i = 0; i < pScene->mNumAnimations; ++i)
    {
        j["animations"].push_back(pScene->mAnimations[i]);
    }

    j["root"] = pScene->mRootNode;
}

int main(int argc, char* argv[])
{
    if (argc == 2) 
    {
        const std::string filename = argv[1];

        Assimp::Importer importer;
        const aiScene* pScene = importer.ReadFile(filename,
                aiProcess_Triangulate |
                aiProcess_JoinIdenticalVertices | 
                aiProcess_SortByPType);

        if (!pScene)
        {
            std::cout << "Error: Something went wrong importing scene" << std::endl;
            std::cout << importer.GetErrorString() << std::endl;
            return 1;
        }

        json j = pScene;

        std::cout << std::setw(4) << j << std::endl;

        return 0;
    }

    std::cout << "Error: Just give me one model filepath" << std::endl;
    return 1;
}
