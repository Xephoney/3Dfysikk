#include "visuallineobject.h"

VisualLineObject::VisualLineObject() : VisualObject("plainshader")
{

}

VisualLineObject::~VisualLineObject()
{
    mVertices.clear();
    mIndices.clear();
}

void VisualLineObject::init(GLint matrixUniform)
{
    mMatrixUniform = matrixUniform;
    VisualObject::init();
    initializeOpenGLFunctions();
    //Vertex Array Object - VAO
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );
    //Vertex Buffer Object to hold vertices - VBO
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );
    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_STATIC_DRAW );
    // 1st attribute buffer : Position
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // 2nd attribute buffer : Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof( Vertex ),  (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,  sizeof( Vertex ),  (GLvoid*)(2 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_color) );
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
}

void VisualLineObject::draw(const glm::mat4 &pMat)
{
    VisualObject::draw(pMat);

    mModelMatrix = pMat * getTransform();
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, glm::value_ptr(mModelMatrix));
    glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)mVertices.size());
}

void VisualLineObject::setLineColor(const glm::vec4 &col)
{
    for(auto iter = mVertices.begin(); iter!=mVertices.end();iter++)
    {
        (*iter).setVertexColor(col.x,col.y,col.z);
    }
}
