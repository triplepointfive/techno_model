#include <assert.h>

#include "mesh.h"

Mesh::MeshEntry::MeshEntry()
{
    VB = INVALID_OGL_VALUE;
    IB = INVALID_OGL_VALUE;
    NumIndices = 0;
    MaterialIndex = INVALID_MATERIAL;
}

Mesh::MeshEntry::~MeshEntry()
{
    if (VB != INVALID_OGL_VALUE){
        glDeleteBuffers(1, &VB);
    }

    if (IB != INVALID_OGL_VALUE){
        glDeleteBuffers(1, &IB);
    }
}

bool Mesh::MeshEntry::Init(const std::vector<Vertex>& Vertices,
                      const std::vector<unsigned int>& Indices)
{
    NumIndices = Indices.size();

    glGenBuffers(1, &VB);
        glBindBuffer(GL_ARRAY_BUFFER, VB);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*Vertices.size(),
                    &Vertices[0], GL_STATIC_DRAW);

    glGenBuffers(1, &IB);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*NumIndices,
                    &Indices[0], GL_STATIC_DRAW);

    return true;
}

void Mesh::Clear()
{
    for (unsigned int i = 0; i < m_Textures.size(); i++){
        SAFE_DELETE(m_Textures[i]);
    }
}

bool Mesh::LoadMesh(const std::string& Filename)
{
//    printf("\n\nMesh::LoadMesh\n");
    // Удаляем данные предыдущей модели (если она была загружена)
    Clear();
//    printf("1\n");

    bool Ret = false;

    Assimp::Importer Importer;
//    printf("2\n");

    const aiScene* pScene = Importer.ReadFile(Filename.c_str(),
                                    aiProcess_Triangulate | aiProcess_GenSmoothNormals|
                                    aiProcess_FlipUVs);

//    printf("3\n");

    if (pScene){
//        printf("Successfully parsed\n");
        Ret = InitFromScene(pScene, Filename);
    } else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
    }

//    printf("End\n\n");
    return Ret;
}

bool Mesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
//    printf("Mesh::InitFromScene %d\n", pScene->mNumMeshes);
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);

    // Инициализируем меши один за другим
    for (unsigned int i = 0; i < m_Entries.size(); i++){
//        printf("Inside\n");
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }

    return InitMaterials(pScene, Filename);
}

void Mesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
    m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;

    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    for (unsigned int i = 0; i< paiMesh->mNumVertices; i++){
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ?
                            &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

        Vertex v(Vector3f(pPos->x, pPos->y, pPos->z),
                 Vector2f(pTexCoord->x,pTexCoord->y),
                 Vector3f(pNormal->x, pNormal->y, pNormal->z));
        Vertices.push_back(v);
    }

    for (unsigned int i = 0; i < paiMesh->mNumFaces; i++){
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }

    m_Entries[Index].Init(Vertices, Indices);
}

bool Mesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Извлекаем директорию из полного имени файла
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;

    if (SlashIndex == std::string::npos){
        Dir = ".";
    }
    else if (SlashIndex == 0){
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Инициализируем материал
    for (unsigned int i = 0; i< pScene->mNumMaterials; i++){
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        m_Textures[i] = NULL;
        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0){
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path,
                                NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS){
                std::string FullPath = Dir + "/" + Path.data;
                m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());

                if (!m_Textures[i]->Load()){
                    printf("Error loading texture '%s'\n", FullPath.c_str());
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
            }
        }

        // Загружаем белую текстуру если модель не имеет собственной
        if (!m_Textures[i]){
            m_Textures[i] = new Texture(GL_TEXTURE_2D, "./white.png");
            Ret = m_Textures[i]->Load();
        }
    }

    return Ret;
}

void Mesh::Render()
{
//    printf("Mesh::Render()\n");
    glEnableVertexAttribArray(0);
//    printf("glEnableVertexAttribArray(0)\n");
    glEnableVertexAttribArray(1);
//    printf("glEnableVertexAttribArray(1)\n");
    glEnableVertexAttribArray(2);
//    printf("glEnableVertexAttribArray(2)\n");
//    printf("size: %lu\n", m_Entries.size());

    for (unsigned int i = 0; i < m_Entries.size(); i++){
//        printf("0\n");
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
//        printf("1\n");
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
//        printf("2\n");
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
//        printf("3\n");
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
//        printf("4\n");

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);
//        printf("5\n");

        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;
//        printf("6\n");

//        printf("if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]){\n");
//        printf("7\n");
        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]){
//            printf("8\n");
            m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }

//        printf("glDrawElements\n");
        glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
    }

    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
}
