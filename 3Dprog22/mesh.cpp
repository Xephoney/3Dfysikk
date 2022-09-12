#include "mesh.h"
#include "assimp/postprocess.h"
#include "assimp/Importer.hpp"
#include "shadercoordinator.h"

Mesh::Mesh() : VisualObject("blinn_phongshader")
{

}

Mesh::~Mesh()
{

}

void Mesh::init()
{

    VisualObject::init();

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );
    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_DYNAMIC_DRAW );

    // 1st attribute buffer : Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(0);

    // 2bd attribute buffer : Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal) );
    glEnableVertexAttribArray(1);

    // 3nd attribute buffer : UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_uv) );
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_color) );
    glEnableVertexAttribArray(3);

    //Index buffer binding so that its easier to create objects without duplicate vertices.
    glGenBuffers( 1, &mIBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, mIndices.size()*sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW );
    glBindVertexArray(0);

}

void Mesh::draw(const glm::mat4 &pMat)
{
    initializeOpenGLFunctions();
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, mVertices.size() * sizeof(Vertex), mVertices.data());
    VisualObject::draw(pMat);
    for(int i = 0; i < (int)mTextures.size(); i++)
    {
        unsigned int diffuseIndex = 0;
        std::string temp;

        // Her legger man til flere texture typer.
        switch(mTextures[i]->type)
        {
        case aiTextureType_DIFFUSE :
            temp = "diffuse" + std::to_string(diffuseIndex++);
            break;
        default :
            break;
        }

        mShader->SetUniform1i(temp, i);
        mTextures[i]->Bind(i);
    }
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, glm::value_ptr(mModelMatrix));
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
}

void Mesh::drawcam(const glm::mat4& mat)
{
    initializeOpenGLFunctions();
    //Sjekker om det er subObjecter slik at vi kan kalle Draw på de først. Vi gjør en depthtest
    // så det har ingenting å si hvilket objekt som blir rendret først, siden det er dybden som bestemmer pixel farge
    mModelMatrix = getTransform() * mat;
    for(VisualObject* a : mSubObjects)
        if(a != nullptr)
            a->draw(mModelMatrix);

    mShader->use();
    //mMatrixUniform = mShader->GetUniformLocation("model");
    glBindVertexArray( mVAO );
    for(int i = 0; i < (int)mTextures.size(); i++)
    {
        mShader->use();
        unsigned int diffuseIndex = 0;
        std::string temp;

        // Her legger man til flere texture typer.
        switch(mTextures[i]->type)
        {
        case aiTextureType_DIFFUSE :
            temp = "diffuse" + std::to_string(diffuseIndex++);
            break;
        default :
            break;
        }

        mShader->SetUniform1i(temp, i);
        mTextures[i]->Bind(i);
    }
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, glm::value_ptr(mModelMatrix));
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
}

void Mesh::loadMesh(std::string filePath, bool loadTexture)
{
    loadTex = loadTexture;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

    //sjekke om filen ble importert rett
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        qDebug() << "Failed to load mesh at (" << filePath.c_str() << ")\n" << importer.GetErrorString();
        return;
    }

    lastetMeshPath = filePath.substr(0, filePath.find_last_of("/"));
    processNode(scene->mRootNode, scene);
}

void Mesh::processNode(aiNode *node, const aiScene *scene)
{
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
    }

    if(node->mNumChildren > 1)
        for(unsigned int i = 0; i < node->mNumChildren; i++)
        {
            Mesh* obj = new Mesh();
            obj->lastetMeshPath = lastetMeshPath;
            obj->loadTex = loadTex;
            mSubObjects.push_back(obj);

            obj->processNode(node->mChildren[i], scene);
        }
    else if(node->mNumChildren != 0)
        processNode(node->mChildren[0], scene);

}

void Mesh::processMesh(aiMesh *mesh, const aiScene *scene)
{
    //henter navnet fra meshen og gir det navnet til VisualObject->mNavn
    mNavn = mesh->mName.C_Str();
    CollisionPolygon KollisjonsPrimitiv (ct);

    // ~0 er en full float tydeligvis? O.o funker fett



    //Fyller ut vertexer
    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        glm::vec3 pos ( mesh->mVertices[i].x,
                        mesh->mVertices[i].y,
                        mesh->mVertices[i].z );

        //Denne loopen gå igjennom og finner min og max av hele meshen som
        //  skal bli brukt til Region KollisjonsPrimitiv

        glm::vec3 normal{0.f};
        if(mesh->HasNormals())
        {
            normal = glm::vec3( mesh->mNormals[i].x,
                               mesh->mNormals[i].y,
                               mesh->mNormals[i].z) ;
        }

        glm::vec2 TextureCoord;
        glm::vec3 color;
        if(mesh->mTextureCoords[0])
        {
            TextureCoord = glm::vec2( mesh->mTextureCoords[0][i].x,
                                      mesh->mTextureCoords[0][i].y );
            color = glm::vec3(1.f);
        }
        else
        {
            TextureCoord = glm::vec2(0.f);
            color = glm::vec3 (1.f);
        }

        mVertices.push_back({pos.x, pos.y, pos.z,
                             normal.x, normal.y, normal.z,
                             TextureCoord.x, TextureCoord.y});
    }

//    glm::vec3 min(FLT_MAX);
//    glm::vec3 max(-FLT_MAX);

//    for(int i = 0; i < (int)mVertices.size(); i++)
//    {
//        for(int j = 0; j < 3; j++)
//        {
//            glm::vec3 t = mVertices[i].m_xyz;
//            if(t[j] < min[j])
//                min[j] = t[j];
//            if(t[j] > max[j])
//                max[j] = t[j];
//        }
//    }

//    //Her oppdaterer vi kollisjons primitiven
//    if(primitiv == CollisionType::AABB)
//    {
//        KollisjonsPrimitiv.min = min;
//        KollisjonsPrimitiv.max = max;
//        Cube* cube = new Cube(KollisjonsPrimitiv.min,
//                              KollisjonsPrimitiv.max,
//                              KollisjonsPrimitiv.calculateCenter());

//        cube->init(0);
//        mSubObjects.push_back(cube);

//    }
//    else
//    {
//        KollisjonsPrimitiv.center = CollisionPolygon(min,max).calculateCenter();
//        glm::vec3 mid = KollisjonsPrimitiv.center;
//        float maxRadius = 0.f;
//        for(uint32_t i = 0; i < mesh->mNumVertices;i++)
//        {
//            float dist = glm::distance2(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z), mid);
//            if(dist > maxRadius)
//                maxRadius = dist;
//        }
//        KollisjonsPrimitiv.radius = sqrt(maxRadius);
//    }



    //Henter indekser
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            mIndices.push_back(face.mIndices[j]);
    }

    if(loadTex)
    {
        //Henter ut detaljer fra materialet
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        //Diffuse textures! :)
        loadTextures(material, aiTextureType_DIFFUSE);
        //her kan vi legge til Normals, Specular etc senere. Helt fantastisk video det her.
        // se link helt øverst i .cpp filen
    }

}

void Mesh::loadTextures(aiMaterial *mat, aiTextureType type)
{
    if(mat->GetTextureCount(type) > 0)
        mShader = ShaderCoordinator::getInstance().fetchShader("blinn_phongtextureshader");

    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for(unsigned int j = 0; j < mTextures.size(); j++)
        {
            if(std::strcmp(mTextures[j]->path.data(), str.C_Str()) == 0)
            {
                skip = true;
                break;
            }
        }

        if(!skip)
        {
            Texture* tex = new Texture(lastetMeshPath, str.C_Str(), type);
            tex->load(false, GL_NEAREST_MIPMAP_NEAREST);
            mTextures.push_back(tex);
        }
    }
}
