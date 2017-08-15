#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <json/json.hpp>

#include <iostream>

using json = nlohmann::json;

json mesh_to_json(const aiMesh* pMesh) {}
json material_to_json(const aiMaterial* pMaterial) {}
json texture_to_json(const aiTexture* pTexture) {}
json light_to_json(const aiLight* pLight) {}
json camera_to_json(const aiCamera* pCamera) {}
json animation_to_json(const aiAnimation* pAnimation) {}
json node_to_json(const aiNode* pNode) {}

void scene_to_json(const aiScene* pScene, json& j)
{
    j["flags"] = pScene->mFlags;

    j["num_meshes"] = pScene->mNumMeshes;
    for (unsigned int i = 0; i < pScene->mNumMeshes; ++i)
    {
        j["meshes"].push_back(mesh_to_json(pScene->mMeshes[i]));
    }

    j["num_materials"] = pScene->mNumMaterials;
    for (unsigned int i = 0; i < pScene->mNumMaterials; ++i)
    {
        j["materials"].push_back(material_to_json(pScene->mMaterials[i]));
    }

    j["num_textures"] = pScene->mNumTextures;
    for (unsigned int i = 0; i < pScene->mNumTextures; ++i)
    {
        j["textures"].push_back(texture_to_json(pScene->mTextures[i]));
    }

    j["num_lights"] = pScene->mNumLights;
    for (unsigned int i = 0; i < pScene->mNumLights; ++i)
    {
        j["lights"].push_back(light_to_json(pScene->mLights[i]));
    }

    j["num_cameras"] = pScene->mNumCameras;
    for (unsigned int i = 0; i < pScene->mNumCameras; ++i)
    {
        j["cameras"].push_back(camera_to_json(pScene->mCameras[i]));
    }

    j["num_animations"] = pScene->mNumAnimations;
    for (unsigned int i = 0; i < pScene->mNumAnimations; ++i)
    {
        j["animations"].push_back(animation_to_json(pScene->mAnimations[i]));
    }

    j["root"] = node_to_json(pScene->mRootNode);
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

        json j;
        scene_to_json(pScene, j);

        std::cout << std::setw(4) << j << std::endl;

        return 0;
    }

    std::cout << "Error: Just give me one model filepath" << std::endl;
    return 1;
}
