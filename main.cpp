#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <iostream>

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

        return 0;
    }

    std::cout << "Error: Just give me one model filepath" << std::endl;
    return 1;
}
