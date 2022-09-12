#include "gridplane.h"

void GridPlane::init(GLint matrixUniform)
{
    VisualObject::init();

    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );
    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );
    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_STATIC_DRAW );
    // 1st attribute buffer : Position

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_xyz));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_color) );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void GridPlane::draw(const glm::mat4& pMat)
{
    if(!DebugDisplay)
        return;
    VisualObject::draw(pMat);
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, glm::value_ptr(mModelMatrix));

    if(displayVerticalAxis)
        glDrawArrays(GL_LINES, 0, mVertices.size());
    else
        glDrawArrays(GL_LINES, 0, mVertices.size()-2);
}

GridPlane::GridPlane(int size) : VisualObject("plainshader")
{
    mNavn = "Grid";
    const int sizeHalf = size/2;

    const float primaryTint = 0.4f;
    //const float secondaryTint = 0.3f;

    int steps = 1;

    //z axis
    for(int x = -sizeHalf; x < sizeHalf+1; x+=steps)
    {
        if(x == 0)
        {
            mVertices.push_back({(float)x,  0,(float)-sizeHalf,0,0,0,0,0,0,0,1});
            mVertices.push_back({(float)x,  0,(float)sizeHalf ,0,0,0,0,0,0,0,1});
        }
        else
        {
            mVertices.push_back({(float)x,  0, (float)-sizeHalf,  0,0,0,0,0,primaryTint,  primaryTint, primaryTint});
            mVertices.push_back({(float)x,  0, (float) sizeHalf,  0,0,0,0,0,primaryTint,  primaryTint, primaryTint});
        }
        if(x == sizeHalf)
            break;
//        mVertices.push_back({(float)x+0.5f, 0, (float)-sizeHalf, 0,0,0,0,0, secondaryTint, secondaryTint, secondaryTint});
//        mVertices.push_back({(float)x+0.5f, 0, (float) sizeHalf, 0,0,0,0,0, secondaryTint, secondaryTint, secondaryTint});
    }

    // x Axis
    for(int z = -sizeHalf; z < sizeHalf+1; z+=steps)
    {
        if(z == 0)
        {
            mVertices.push_back({(float)-sizeHalf,0,0,0,0,0,0,(float)z,1,0,0});
            mVertices.push_back({(float) sizeHalf,0,0,0,0,0,0,(float)z,1,0,0});
        }
        else
        {
            mVertices.push_back({(float)-sizeHalf,0,(float)z     ,0,0,0,0,0,  primaryTint,  primaryTint, primaryTint});
            mVertices.push_back({(float) sizeHalf,0,(float)z     ,0,0,0,0,0,  primaryTint,  primaryTint, primaryTint});
        }

        if(z == sizeHalf)
            break;
//        mVertices.push_back({ (float)-sizeHalf, 0, (float)z + 0.5f, 0,0,0,0,0, secondaryTint, secondaryTint, secondaryTint});
//        mVertices.push_back({ (float) sizeHalf, 0, (float)z + 0.5f, 0,0,0,0,0, secondaryTint, secondaryTint, secondaryTint});
    }

    // y Axis
    mVertices.push_back({0,          0    , 0, 0,0,0,0,0, 0, 1, 0});
    mVertices.push_back({0,(float)sizeHalf, 0, 0,0,0,0,0, 0, 1, 0});
}

GridPlane::~GridPlane()
{

}
