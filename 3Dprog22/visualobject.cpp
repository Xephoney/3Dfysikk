
#include "visualobject.h"
#include "toolbox.h"
#include "shadercoordinator.h"

// Mesh innlasting med Assimp ble lagd ved hjelp av denne tutorialen :
// link : https://www.youtube.com/watch?v=jjlOLKkhckU

VisualObject::VisualObject() : ct(CollisionType::AABB), cp(ct), cr(CollisionResponse::GHOST)
{
    ShaderCoordinator& shaderCoord = ShaderCoordinator::getInstance();
    mShader = shaderCoord.fetchShader("plainshader");
    mShader->use();
}

VisualObject::VisualObject(std::string shaderName, CollisionResponse cr ) : cp(CollisionType::AABB), cr(cr)
{
    ShaderCoordinator& shaderCoord = ShaderCoordinator::getInstance();
    mShader = shaderCoord.fetchShader(shaderName);

    mShader->use();
    mMatrixUniform = mShader->GetUniformLocation("model");

}

VisualObject::~VisualObject()
{
    for(int i = 0; i < mSubObjects.size(); i++)
        delete mSubObjects.at(i);

    glDeleteVertexArrays( 1, &mVAO );
    glDeleteBuffers( 1, &mVBO );
}

void VisualObject::init()
{
    initializeOpenGLFunctions();
    mShader->use();
    mMatrixUniform = mShader->GetUniformLocation("model");

    for(auto& obj : mSubObjects)
        obj->init(mMatrixUniform);

    startPos    = getPosition();
    startRot    = getRotation();
    startScale  = getScale();
}

void VisualObject::init(GLint matrixUniform)
{
    init();
//Dette er her så jeg slipper warningen om at det er en ubrukt variabel her :/

    if(mVertices.size() > 0)
    {
        //Vertex Array Object - VAO
        glGenVertexArrays( 1, &mVAO );
        glBindVertexArray( mVAO );

        //Vertex Buffer Object to hold vertices - VBO
        glGenBuffers( 1, &mVBO );
        glBindBuffer( GL_ARRAY_BUFFER, mVBO );
        glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_STATIC_DRAW );

        // 1st attribute buffer : Position
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal) );
        glEnableVertexAttribArray(1);

        // 3nd attribute buffer : UV
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_uv) );
        glEnableVertexAttribArray(2);

        //Index buffer binding so that its easier to create objects without duplicate vertices.
        glGenBuffers( 1, &mIBO );
        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, mIndices.size()*sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW );
        glBindVertexArray(0);
    }
}

void VisualObject::draw(const glm::mat4& pMat)
{
    initializeOpenGLFunctions();
    mModelMatrix = pMat * getTransform();
    //Sjekker om det er subObjecter slik at vi kan kalle Draw på de først. Vi gjør en depthtest
    // så det har ingenting å si hvilket objekt som blir rendret først, siden det er dybden som bestemmer pixel farge
    for(VisualObject* a : mSubObjects)
        if(a != nullptr)
            a->draw(mModelMatrix);

    mShader->use();
    //mMatrixUniform = mShader->GetUniformLocation("model");
    if(mVertices.size()>0)
        glBindVertexArray( mVAO );
}

    //Oppgave 7, 8 og 9
void VisualObject::tick(const float &dt)
{
    timeSinceCreation+=dt;
    if(cr == CollisionResponse::DYNAMIC)
    {
        vertexPositions.clear();
        fetchVertices(vertexPositions);
        findMinMax(cp.min,cp.max,vertexPositions);
        cp.center = mPosition;
    }
}

void VisualObject::reset()
{
    mPosition = startPos;
    mRotation = startRot;
    mScale = startScale;
    for(auto subObj : mSubObjects)
        subObj->reset();
    mModelMatrix = getTransform();
}

void VisualObject::setColor(const float &r, const float &g, const float &b, bool recursive)
{
    if(recursive)
        for(auto& obj : mSubObjects)
            obj->setColor(r,g,b,recursive);

    setColor(r,g,b);

}

void VisualObject::setShader(Shader *shader)
{
    if(!shader)
        return;

    mShader = shader;
    mShader->use();
    mMatrixUniform = mShader->GetUniformLocation("model");
}

void VisualObject::setShader(const std::string &shader)
{
    setShader(ShaderCoordinator::getInstance().fetchShader(shader));
}

void VisualObject::normalizeVertexNormals()
{
    for(auto it = mVertices.begin(); it != mVertices.end(); it++)
        if(glm::length((*it).m_normal) > 0)
        {
            (*it).m_normal = glm::normalize((*it).m_normal);
        }
}

void VisualObject::ToggleDebugDisplay(bool b)
{
    for(auto& obj : mSubObjects)
        obj->ToggleDebugDisplay(b);

    if(std::strcmp("BomberPath",mNavn.c_str()) == 0)
        return;

    DebugDisplay = b;
}


glm::mat4 VisualObject::getTransform()
{
    return createTransformMatrix(mPosition, mRotation, mScale);
}

//Oppgave 7, 8 og 9
void VisualObject::fetchVertices(std::vector<glm::vec3>& vertexPos)
{
    //Får inn en referanse til en vertex-liste
    //legger først inn alle vertices inn i listen med en enkel insert
//    for(const auto& v : mVertices)
//        vertexPos.push_back(v.m_xyz);

    const glm::mat4 transformMat = createTransformMatrix(glm::vec3(0), mRotation, mScale);
    for(int i = 0; i < (int)mVertices.size(); i++)
    {
        glm::vec3 old = mVertices[i].m_xyz;
        glm::vec3 ny = transformMat * glm::vec4(old, 1);
        vertexPos.push_back(ny);
    }
    //Går deretter igjennom alle subObjects
    // og gjentar funksjonen rekursivt
    for(auto& subObj : mSubObjects)
        subObj->fetchVertices(vertexPos);

    // Ideen her ble tatt fra koden Dag skrev i QuadTree koden. se linje 55, men jeg byttet den ut med
    //   insert istedenfor en iterator loop.
}

//Oppgave 7, 8 og 9
void VisualObject::findMinMax(glm::vec3 &min, glm::vec3& max)
{
    min = glm::vec3(FLT_MAX);
    max = glm::vec3(-FLT_MAX);

    const glm::mat4 transformMat = createTransformMatrix(glm::vec3(0), mRotation, mScale);
    for(const auto& v : mVertices)
    {
        glm::vec3 pos = transformMat * glm::vec4(v.m_xyz, 1);
        for(int i = 0; i < 3; i++)
        {
            if(pos[i] < min[i])
                min[i] = pos[i];
            if(pos[i] > max[i])
                max[i] = pos[i];
        }
    }
}

//Oppgave 7, 8 og 9
void VisualObject::findMinMax(glm::vec3 &min, glm::vec3 &max, std::vector<glm::vec3>& vertexPos)
{
    min = glm::vec3(FLT_MAX);
    max = glm::vec3(-FLT_MAX);

    for(const auto& v : vertexPos)
    {
        for(int i = 0; i < 3; i++)
        {
            if(v[i] < min[i])
                min[i] = v[i];
            if(v[i] > max[i])
                max[i] = v[i];
        }
    }
}

void VisualObject::initCollisions()
{
    if(cr != CollisionResponse::GHOST)
    {
        ct = CollisionType::AABB;
        CollisionPolygon colpol (ct);
        colpol.center = mPosition;
        if(mVertices.size() != 0 && mSubObjects.size() == 0)
        {
            findMinMax(colpol.min, colpol.max);
        }
        else
        {
            fetchVertices(vertexPositions);
            findMinMax(colpol.min, colpol.max, vertexPositions);
        }

        cp = colpol;
    }
}


