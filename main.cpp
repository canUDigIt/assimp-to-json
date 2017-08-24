#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <json/json.hpp>

#include <iostream>
#include <string>

using json = nlohmann::json;

void to_json(json& j, const aiString& s)
{
    j = s.C_Str();
}

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
        {"name", pBone->mName},
        {"num_weights", pBone->mNumWeights},
        {"offset_matrix", pBone->mOffsetMatrix},
        {"weights", json::array()}
    };

    for (int i = 0; i < pBone->mNumWeights; ++i)
    {
        j["weights"].push_back(pBone->mWeights[i]);
    }
}

void to_json(json& j, const aiVector2D& vertex)
{
    j = json {vertex.x, vertex.y};
}

void to_json(json& j, const aiVector3D& vertex)
{
    j = json {vertex.x, vertex.y, vertex.z};
}

void to_json(json& j, const aiColor4D& color)
{
    j = json {color.r, color.g, color.b, color.a};
}

void to_json(json& j, const aiColor3D& color)
{
    j = json {color.r, color.g, color.b};
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
    j["name"] = pMesh->mName;
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

std::string texture_string(const aiTextureType& type)
{
    std::string name;

    switch (type)
    {
        case aiTextureType_DIFFUSE:
            name = "diffuse";
            break;
        case aiTextureType_SPECULAR:
            name = "specular";
            break;
        case aiTextureType_AMBIENT:
            name = "ambient";
            break;
        case aiTextureType_EMISSIVE:
            name = "emissive";
            break;
        case aiTextureType_HEIGHT:
            name = "height";
            break;
        case aiTextureType_NORMALS:
            name = "normals";
            break;
        case aiTextureType_SHININESS:
            name = "shininess";
            break;
        case aiTextureType_OPACITY:
            name = "opacity";
            break;
        case aiTextureType_DISPLACEMENT:
            name = "displacement";
            break;
        case aiTextureType_LIGHTMAP:
            name = "lightmap";
            break;
        case aiTextureType_REFLECTION:
            name = "reflection";
            break;
        case aiTextureType_UNKNOWN:
            name = "unknown";
            break;
        default:
            name = "none";
            break;
    };

    return name;
}

void to_json(json& j, const aiMaterial* pMaterial)
{
    aiString name;
    if (pMaterial->Get(AI_MATKEY_NAME, name) == AI_SUCCESS)
    {
        j["name"] = name;
    }

    aiColor3D diffuse(0.f, 0.f, 0.f);
    if (pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS)
    {
        j["diffuse"] = diffuse;
    }

    aiColor3D specular(0.f, 0.f, 0.f);
    if (pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular) == AI_SUCCESS)
    {
        j["specular"] = specular;
    }

    aiColor3D ambient(0.f, 0.f, 0.f);
    if (pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient) == AI_SUCCESS)
    {
        j["ambient"] = ambient;
    }

    aiColor3D emissive(0.f, 0.f, 0.f);
    if (pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive) == AI_SUCCESS)
    {
        j["emissive"] = emissive;
    }

    aiColor3D trans(0.f, 0.f, 0.f);
    if (pMaterial->Get(AI_MATKEY_COLOR_TRANSPARENT, trans)== AI_SUCCESS)
    {
        j["transparent"] = trans;
    }

    int wireframe = 0;
    if (pMaterial->Get(AI_MATKEY_ENABLE_WIREFRAME, wireframe) == AI_SUCCESS)
    {
        j["wireframe"] = wireframe == 0 ? false : true;
    }

    int twosided = 0;
    if (pMaterial->Get(AI_MATKEY_TWOSIDED, twosided) == AI_SUCCESS)
    {
        j["twosided"] = twosided == 0 ? false : true;
    }

    int shading_model = 0;
    if (pMaterial->Get(AI_MATKEY_SHADING_MODEL, shading_model) == AI_SUCCESS)
    {
        j["shading_model"] = shading_model;
    }

    int blend_func = 0;
    if (pMaterial->Get(AI_MATKEY_BLEND_FUNC, blend_func) == AI_SUCCESS)
    {
        j["blend_func"] = blend_func;
    }

    float opacity = 1.f;
    if (pMaterial->Get(AI_MATKEY_OPACITY, opacity) == AI_SUCCESS)
    {
        j["opacity"] = opacity;
    }

    float shininess = 0.f;
    if (pMaterial->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
    {
        j["shininess"] = shininess;
    }

    float shininess_strength = 1.f;
    if (pMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, shininess_strength) == AI_SUCCESS)
    {
        j["shininess_strength"] = shininess_strength;
    }

    float refraction = 1.f;
    if (pMaterial->Get(AI_MATKEY_REFRACTI, refraction) == AI_SUCCESS)
    {
        j["refraction"] = refraction;
    }

    std::vector<json> textures;
    unsigned int texture_type_count = static_cast<int>(aiTextureType_UNKNOWN) + 1;
    for (unsigned int i = 1; i < texture_type_count; ++i)
    {
        aiTextureType type = static_cast<aiTextureType>(i);

        unsigned int count = pMaterial->GetTextureCount(type);
        for (unsigned int index = 0; index < count; ++index)
        {
            aiString path;
            aiTextureMapping mapping = aiTextureMapping_UV;
            unsigned int uvindex = 0;
            ai_real blend = 1.f;
            aiTextureOp op = aiTextureOp_Multiply;
            std::vector<aiTextureMapMode> mapmode(3);

            if (pMaterial->GetTexture(type, index, &path, &mapping, &uvindex, &blend, &op, mapmode.data()) == AI_SUCCESS)
            {
                textures.push_back(json {
                    {"type", texture_string(type)},
                    {"path", path},
                    {"mapping", static_cast<unsigned int>(mapping)},
                    {"uvindex", uvindex},
                    {"blend", blend},
                    {"op", static_cast<unsigned int>(op)},
                    {"mapmode", mapmode}
                });
            }
        }
    }

    if (textures.empty() == false)
    {
        j["textures"] = textures;
    }
}

void to_json(json& j, const aiTexel& texel)
{
    j = json {texel.a, texel.b, texel.g, texel.r};
}

void to_json(json& j, const aiTexture* pTexture)
{
    std::vector<aiTexel> data(pTexture->mWidth * pTexture->mHeight);
    std::memcpy(data.data(), pTexture->pcData, data.size() * sizeof(aiTexel));

    j = json {
        {"format", pTexture->achFormatHint},
        {"height", pTexture->mHeight},
        {"width", pTexture->mWidth},
        {"data", data}
    };
}

void to_json(json& j, const aiLight* pLight)
{
    j = json {
        {"name", pLight->mName},
        {"type", static_cast<unsigned int>(pLight->mType)},
        {"position", pLight->mPosition},
        {"direction", pLight->mDirection},
        {"up", pLight->mUp},
        {"inner_cone_angle", pLight->mAngleInnerCone},
        {"outer_cone_angle", pLight->mAngleOuterCone},
        {"attenuation_constant", pLight->mAttenuationConstant},
        {"attenuation_linear", pLight->mAttenuationLinear},
        {"attenuation_quadratic", pLight->mAttenuationQuadratic},
        {"color_ambient", pLight->mColorAmbient},
        {"color_diffuse", pLight->mColorDiffuse},
        {"color_specular", pLight->mColorSpecular},
        {"size", pLight->mSize}
    };
}

void to_json(json& j, const aiCamera* pCamera)
{
    j = json {
        {"name", pCamera->mName},
        {"position", pCamera->mPosition},
        {"lookAt", pCamera->mLookAt},
        {"up", pCamera->mUp}, 
        {"aspect", pCamera->mAspect},
        {"far", pCamera->mClipPlaneFar},
        {"near", pCamera->mClipPlaneNear},
        {"horizontalFOV", pCamera->mHorizontalFOV}
    };
}

void to_json(json& j, const aiVectorKey& key)
{
    j = json {
        {"time", key.mTime},
        {"value", key.mValue}
    };
}

void to_json(json& j, const aiQuaternion& q)
{
    j = json { q.x, q.y, q.z, q.w };
}

void to_json(json& j, const aiQuatKey& key)
{
    j = json {
        {"time", key.mTime},
        {"value", key.mValue}
    };
}

void to_json(json& j, const aiNodeAnim* nodeAnim)
{
    unsigned int position_count = nodeAnim->mNumPositionKeys;
    unsigned int rotation_count = nodeAnim->mNumRotationKeys;
    unsigned int scaling_count = nodeAnim->mNumScalingKeys;

    std::vector<aiVectorKey> position_keys(position_count);
    std::vector<aiQuatKey> rotation_keys(rotation_count);
    std::vector<aiVectorKey> scaling_keys(scaling_count);

    std::memcpy(position_keys.data(), nodeAnim->mPositionKeys, sizeof(aiVectorKey) * position_count);
    std::memcpy(rotation_keys.data(), nodeAnim->mRotationKeys, sizeof(aiQuatKey) * rotation_count);
    std::memcpy(scaling_keys.data(), nodeAnim->mScalingKeys, sizeof(aiVectorKey) * scaling_count);

    j = json {
        {"node_name", nodeAnim->mNodeName},
        {"num_position_keys", position_count},
        {"num_rotation_keys", rotation_count},
        {"num_scaling_keys", scaling_count},
        {"position_keys", position_keys},
        {"post_state", static_cast<unsigned int>(nodeAnim->mPostState)},
        {"pre_state", static_cast<unsigned int>(nodeAnim->mPreState)},
        {"rotation_keys", rotation_keys},
        {"scaling_keys", scaling_keys}
    };
}

void to_json(json& j, const aiMeshKey& key)
{
    j = json {
        {"time", key.mTime},
        {"value", key.mValue}
    };
}

void to_json(json& j, const aiMeshAnim* pMeshAnim)
{
    unsigned int count = pMeshAnim->mNumKeys;
    std::vector<aiMeshKey> keys(count);
    std::memcpy(keys.data(), pMeshAnim->mKeys, sizeof(aiMeshKey) * count);

    j = json {
        {"keys", keys},
        {"name", pMeshAnim->mName},
        {"num_keys", count}
    };
}

void to_json(json& j, const aiMeshMorphKey& key)
{
    unsigned int count = key.mNumValuesAndWeights;
    std::vector<unsigned int> values(count);
    std::vector<double> weights(count);

    std::memcpy(values.data(), key.mValues, sizeof(unsigned int) * count);
    std::memcpy(weights.data(), key.mWeights, sizeof(double) * count);

    j = json {
        {"num_values_and_weights", count},
        {"time", key.mTime},
        {"values", values},
        {"weights", weights}
    };
}

void to_json(json& j, const aiMeshMorphAnim* pMeshMorphAnim)
{
    unsigned int count = pMeshMorphAnim->mNumKeys;
    std::vector<aiMeshMorphKey> keys(count);
    std::memcpy(keys.data(), pMeshMorphAnim->mKeys, sizeof(aiMeshMorphKey) * count);

    j = json {
        {"keys", keys},
        {"name", pMeshMorphAnim->mName},
        {"num_keys", count}
    };
}

void to_json(json& j, const aiAnimation* pAnimation)
{
    unsigned int c_count = pAnimation->mNumChannels;
    unsigned int mc_count = pAnimation->mNumMeshChannels;
    unsigned int mmc_count = pAnimation->mNumMorphMeshChannels;

    std::vector<aiNodeAnim*> nodeAnims(c_count);
    std::memcpy(nodeAnims.data(), pAnimation->mChannels, sizeof(aiNodeAnim*) * c_count);

    std::vector<aiMeshAnim*> meshAnims(mc_count);
    std::memcpy(meshAnims.data(), pAnimation->mMeshChannels, sizeof(aiMeshAnim*) * mc_count);
    
    std::vector<aiMeshMorphAnim*> meshMorphAnims(mmc_count);
    std::memcpy(meshMorphAnims.data(), pAnimation->mMorphMeshChannels, sizeof(aiMeshMorphAnim*) * mmc_count);

    j = json {
        {"channels", nodeAnims},
        {"duration", pAnimation->mDuration},
        {"mesh_channels", meshAnims},
        {"morph_mesh_channels", meshMorphAnims},
        {"name", pAnimation->mName},
        {"num_channels", c_count},
        {"num_mesh_channels", mc_count},
        {"num_morph_mesh_channels", mmc_count},
        {"ticks_per_second", pAnimation->mTicksPerSecond}
    };
}

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
