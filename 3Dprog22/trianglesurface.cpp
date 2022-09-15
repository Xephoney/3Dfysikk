#include "trianglesurface.h"
#include <math.h>
#include <fstream>
#include "WorldCoordinator.h"
#include "toolbox.h"


TriangleSurface::TriangleSurface() : VisualObject("plainshader")
{

}

TriangleSurface::TriangleSurface(std::string filnavn) : VisualObject("plainshader")
{
    readFile(filnavn);
}

TriangleSurface::~TriangleSurface() { }

void TriangleSurface::writeToFile(std::string filNavn)
{
    std::ofstream file("../3Dprog22/"+filNavn);
    if(file.is_open())
    {
        file << mVertices.size() << "\n";
        for(auto i = 0; i < (int)mVertices.size(); i++)
            file << mVertices[i] << "\n";

        //ferdig skrevet ut til fil.
        file.close();
    }
}

void TriangleSurface::readFile(std::string filnavn)
{
    std::ifstream file;
    file.open("../3Dprog22/"+filnavn);

    if (file.is_open()) {
        // Vertexer
        int n;

        //Indexer
        int m;
        Vertex vertex;
        file >> n;
        mVertices.reserve(n);
        for (int i=0; i<n; i++) {
             file >> vertex;
             vertex.m_color = vertex.m_normal;
             mVertices.push_back(vertex);
        }
        file >> m;
        mIndices.reserve(m);
        triangleCount = m / 3;
        for(int i = 0; i < m; i++)
        {
            int index;
            file >> index;
            mIndices.push_back(index);
        }
        file.close();
    }

    World::getWorld().surface = this;
}

void TriangleSurface::init()
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_normal) );
    glEnableVertexAttribArray(1);


    //Index buffer binding so that its easier to create objects without duplicate vertices.
    glGenBuffers( 1, &mIBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, mIndices.size()*sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW );
    glBindVertexArray(0);
}

void TriangleSurface::draw(const glm::mat4& pMat)
{
    VisualObject::draw(pMat);

    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, glm::value_ptr(mModelMatrix));
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
}

glm::vec3 TriangleSurface::triangleNormal(int triangleIndex)
{
   if(triangleCount == 0 && mIndices.size() == 0)
        return glm::vec3{0.f};
    if(triangleIndex > triangleCount)
        return glm::vec3 {0.f};

    glm::vec3 vPos[3];
    for(int i = 0; i < 3; i++)
    {
        vPos[i] = mVertices[mIndices[triangleIndex * 3 + i]].m_xyz;
    }
    glm::vec3 ba = vPos[1]-vPos[0];
    qDebug() << "vec_a ("<<ba.x<<","<<ba.y<<","<<ba.z<<")";
    glm::vec3 ca = vPos[2]-vPos[0];
    qDebug() << "vec_b ("<<ca.x<<","<<ca.y<<","<<ca.z<<")";
    glm::vec3 n = glm::normalize(glm::cross(ca,ba));
    return n;
}

void TriangleSurface::trianglePositions(int triangleIndex, std::vector<glm::vec3> &vec)
{
    if(triangleIndex<0)
        return;
    for(int i = 0; i < 3; i++)
    {
        //vec[i] = mVertices[triangleIndex * 3 + i].m_xyz;
        vec.push_back(mVertices[mIndices[triangleIndex * 3 + i]].m_xyz);
    }
}

void TriangleSurface::makeTriangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c)
{
    mVertices.push_back(Vertex(a.x,a.y,a.z,a.x,a.y,a.z));
    mVertices.push_back(Vertex(b.x,b.y,b.z,b.x,b.y,b.z));
    mVertices.push_back(Vertex(c.x,c.y,c.z,c.x,c.y,c.z));
    mIndices.push_back((int)mIndices.size());
    mIndices.push_back((int)mIndices.size());
    mIndices.push_back((int)mIndices.size());
}

void TriangleSurface::makeTriangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &farge)
{
    mVertices.push_back(Vertex(a.x,a.y,a.z,farge.x,farge.y,farge.z));
    mVertices.push_back(Vertex(b.x,b.y,b.z,farge.x,farge.y,farge.z));
    mVertices.push_back(Vertex(c.x,c.y,c.z,farge.x,farge.y,farge.z));
    mIndices.push_back((int)mIndices.size());
    mIndices.push_back((int)mIndices.size());
    mIndices.push_back((int)mIndices.size());
}

void TriangleSurface::makeTriangle(const int &indx0, const int &indx1, const int &indx2)
{
    mIndices.push_back(indx0);
    mIndices.push_back(indx1);
    mIndices.push_back(indx2);
}

void TriangleSurface::makeQuad(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &d)
{
    mVertices.push_back(Vertex{ a.x, a.y, a.z, 0.5, 0.5, 0.5, 0, 0});
    mVertices.push_back(Vertex{ b.x, b.y, b.z, 0.5, 0.5, 0.5, 0, 1});
    mVertices.push_back(Vertex{ c.x, c.y, c.z, 0.5, 0.5, 0.5, 1, 1});
    mVertices.push_back(Vertex{ c.x, c.y, c.z, 0.5, 0.5, 0.5, 1, 1});
    mVertices.push_back(Vertex{ d.x, d.y, d.z, 0.5, 0.5, 0.5, 1, 0});
    mVertices.push_back(Vertex{ a.x, a.y, a.z, 0.5, 0.5, 0.5, 0, 0});
}

void TriangleSurface::makeQuad(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, const glm::vec3 &d, const glm::vec3 &farge)
{
    makeTriangle(a, b, c, farge);
    makeTriangle(c, d, a, farge);
}

//TODO: DENNE MÅ FIKSES
void TriangleSurface::construct()
{
    float xmin=-10.0f, xmax=10.0f, zmin=-10.0f, zmax=10.0f, h=0.1f;
    for (auto x=xmin; x<xmax; x+=h)                                                                     //
        for (auto z=zmin; z<zmax; z+=h)                                                                 //
        {
            float y = func(x,z);
            float yCol = remap(y, -2.f,1.2f,0,1);
            float xCol = remap(x, xmin,xmax, 0,1);
            float zCol = remap(z, zmin,zmax, 0,1);
            mVertices.push_back(Vertex{x,y,z,xCol,yCol,zCol});

            y = func(x, z+h);
            yCol = remap(y, -2.f,1.2f,0,1);
            xCol = remap(x, xmin,xmax, 0,1);
            zCol = remap(z+h, zmin,zmax, 0,1);
            mVertices.push_back(Vertex{x,y,z+h,xCol,yCol,zCol});

            y = func(x+h, z+h);
            yCol = remap(y, -2.f,1.2f,0,1);
            xCol = remap(x+h, xmin,xmax, 0,1);
            zCol = remap(z+h, zmin,zmax, 0,1);
            mVertices.push_back(Vertex{x+h,y,z+h,xCol,yCol,zCol});
            mVertices.push_back(Vertex{x+h,y,z+h,xCol,yCol,zCol});

            y = func(x+h, z);
            yCol = remap(y, -2.f,1.2f,0,1);
            xCol = remap(x+h, xmin,xmax, 0,1);
            zCol = remap(z, zmin,zmax, 0,1);
            mVertices.push_back(Vertex{x+h,y,z,xCol,yCol,zCol});

            y = func(x,z);
            yCol = remap(y, -2.f,1.2f,0,1);
            xCol = remap(x, xmin,xmax, 0,1);
            zCol = remap(z, zmin,zmax, 0,1);
            mVertices.push_back(Vertex{x,y,z,xCol,yCol,zCol});
        }
    writeToFile("funksjon.txt");
}
void TriangleSurface::constructIndices()
{
    float xmin=-6.0f, xmax=6.0f, zmin=-6.0f, zmax=6.0f, h=0.1f;

    int xSteps = (xmax-xmin)/h;
    int zSteps = (zmax-zmin)/h;

    for(int i = 0; i < xSteps; i++)
        for(int j = 0; j < zSteps; j++)
        {

        }

    for (auto x=xmin; x<xmax; x+=h)                                                                     //
        for (auto z=zmin; z<zmax; z+=h)                                                                 //
        {
            float y = func(x,z);
            float yCol = remap(y, -2.f,1.2f,0,1);
            float xCol = remap(x, xmin,xmax, 0,1);
            float zCol = remap(z, zmin,zmax, 0,1);
            mVertices.push_back(Vertex{x,y,z,xCol,yCol,zCol});

            y = func(x, z+h);
            yCol = remap(y, -2.f,1.2f,0,1);
            xCol = remap(x, xmin,xmax, 0,1);
            zCol = remap(z+h, zmin,zmax, 0,1);
            mVertices.push_back(Vertex{x,y,z+h,xCol,yCol,zCol});

            y = func(x+h, z+h);
            yCol = remap(y, -2.f,1.2f,0,1);
            xCol = remap(x+h, xmin,xmax, 0,1);
            zCol = remap(z+h, zmin,zmax, 0,1);
            mVertices.push_back(Vertex{x+h,y,z+h,xCol,yCol,zCol});

            y = func(x+h, z);
            yCol = remap(y, -2.f,1.2f,0,1);
            xCol = remap(x+h, xmin,xmax, 0,1);
            zCol = remap(z, zmin,zmax, 0,1);
            mVertices.push_back(Vertex{x+h,y,z,xCol,yCol,zCol});

        }
    writeToFile("funksjonIndex.txt");
}

float TriangleSurface::func(const float x, const float y)
{
    //-pow(x,2)+5-pow(y,2);
    return pow(sin(x),2)*cos(y);
}

