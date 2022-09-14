#include "octahedronball.h"
#include "toolbox.h"

void OctahedronBall::lagTriangel(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3)
{
    float xCol, yCol, zCol;
    xCol=remap(v1.x,-1,1,0,1); //Farge kode
    yCol=remap(v1.y,-1,1,0,1); //Farge kode
    zCol=remap(v1.z,-1,1,0,1); //Farge kode
    mVertices.push_back({v1.x, v1.y, v1.z, v1.x, v1.y, v1.z, 0, 0, xCol, yCol, zCol});
    xCol=remap(v2.x,-1,1,0,1); //Farge kode
    yCol=remap(v2.y,-1,1,0,1); //Farge kode
    zCol=remap(v2.z,-1,1,0,1); //Farge kode
    mVertices.push_back({v2.x, v2.y, v2.z, v2.x, v2.y, v2.z, 0, 0, xCol, yCol, zCol});
    xCol=remap(v3.x,-1,1,0,1); //Farge kode
    yCol=remap(v3.y,-1,1,0,1); //Farge kode
    zCol=remap(v3.z,-1,1,0,1); //Farge kode
    mVertices.push_back({v3.x, v3.y, v3.z, v3.x, v3.y, v3.z, 0, 0, xCol, yCol, zCol});
}

void OctahedronBall::subDivide(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, int n)
{
    if (n>0) {
        glm::vec3 v1 = a+b; v1 = glm::normalize(v1);
        glm::vec3 v2 = a+c; v2 = glm::normalize(v2);
        glm::vec3 v3 = c+b; v3 = glm::normalize(v3);
        subDivide(a, v1, v2, n-1);
        subDivide(c, v2, v3, n-1);
        subDivide(b, v3, v1, n-1);
        subDivide(v3, v2, v1, n-1);
    } else {
        lagTriangel(a, b, c);
    }
}

void OctahedronBall::oktaederUnitBall()
{
   glm::vec3 v0{0, 0, 1};
   glm::vec3 v1{1, 0, 0};
   glm::vec3 v2{0, 1, 0};
   glm::vec3 v3{-1, 0, 0};
   glm::vec3 v4{0, -1, 0};
   glm::vec3 v5{0, 0, -1};

   subDivide(v0, v1, v2, m_iterations);
   subDivide(v0, v2, v3, m_iterations);
   subDivide(v0, v3, v4, m_iterations);
   subDivide(v0, v4, v1, m_iterations);
   subDivide(v5, v2, v1, m_iterations);
   subDivide(v5, v3, v2, m_iterations);
   subDivide(v5, v4, v3, m_iterations);
   subDivide(v5, v1, v4, m_iterations);
}

OctahedronBall::OctahedronBall(): VisualObject("plainshader"), m_iterations(3), m_indeks(0)
{
    mVertices.reserve(3 * 8 * pow(4, m_iterations));
    oktaederUnitBall();
}

OctahedronBall::OctahedronBall(int n, const std::string& shader) : VisualObject(shader), m_iterations(n), m_indeks(0)
{
    mVertices.reserve(3 * 8 * pow(4, m_iterations));
    oktaederUnitBall();


}

OctahedronBall::~OctahedronBall()
{

}

void OctahedronBall::init(GLint matrixUniform)
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
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // 2nd attribute buffer : Normals
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,  sizeof( Vertex ),  (GLvoid*)(3 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(1);

    // 3nd attribute buffer : UV
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,  sizeof( Vertex ),  (GLvoid*)(2 * sizeof(GLfloat)) );
    glEnableVertexAttribArray(2);

    // 4th attribute buffer : color
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_color) );
    glEnableVertexAttribArray(3);

    glGenBuffers( 1, &mIBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, mIndices.size()*sizeof( unsigned int ), mIndices.data(), GL_STATIC_DRAW );
}

void OctahedronBall::draw(const glm::mat4& pMat)
{
    VisualObject::draw(pMat);
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, glm::value_ptr(mModelMatrix));
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)mVertices.size());
}

void OctahedronBall::tick(const float &dt)
{
    velocity += glm::vec3{0,-9.81,0} * dt;
    Translate(velocity*dt);
}
