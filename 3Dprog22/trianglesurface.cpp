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
    //readFile(filnavn);
    readCompressed(filnavn);
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
    qDebug()<<"constructor called \n";
    std::ifstream file;
    file.open("../3Dprog22/"+filnavn);
    if (file.is_open()) {

        //lager midlertidig array som skal holde inndata i det den leses
        float in[3];
        Vertex tempVert;
        Vertex lastVert;
        int linesRead=0;

        do
        {

            //Punkt data fra kartverket er satt opp i XZY,
            //men jeg vil ha det i XYZ, må derfor sette det tredje elementet i fil-linje til andre element i in.
            file>>in[0]>>in[2]>>in[1];

            tempVert=Vertex(in[0],in[1],in[2]);

            if(mVertices.size()<1){
                //har nå denne linjens punktdata
                //sjekker om nye data er max eller min i sin akse
                for(int i=0;i<3;i++){
                    if(in[i]>max[i])
                        max[i]=in[i];
                    if(in[i]<min[i])
                        min[i]=in[i];
                }
                tempVert.m_color=tempVert.m_normal;
                mVertices.push_back(tempVert);
                linesRead=linesRead+1;
                lastVert=tempVert;
            }

            else{
                if(!tooClose(tempVert, lastVert)){
                    for(int i=0;i<3;i++){
                        if(in[i]>max[i])
                            max[i]=in[i];
                        if(in[i]<min[i])
                            min[i]=in[i];
                    }
                    tempVert.m_color=tempVert.m_normal;
                    mVertices.push_back(tempVert);
                    linesRead=linesRead+1;
                    lastVert=tempVert;
                }
            }

        }
        while(!file.eof());

        qDebug()<<"lines read : "<< linesRead;



        for(auto i=0;i<mVertices.size();i++){
            mVertices[i].m_xyz.x=mVertices[i].m_xyz.x-min[0];
            mVertices[i].m_xyz.y=mVertices[i].m_xyz.y-min[1];
            mVertices[i].m_xyz.z=mVertices[i].m_xyz.z-min[2];
        }
        max[0]=max[0]-min[0];
        max[1]=max[1]-min[1];
        max[2]=max[2]-min[2];

        min[0]=min[0]-min[0];
        min[1]=min[1]-min[1];
        min[2]=min[2]-min[2];

        file.close();
    }

    std::pair<float,int> grid[(int)max[0]][(int)max[2]];

    // finne hvilke punkter i gridet alle vertexer er i
    for(auto i=0;i<mVertices.size();i++){
        int x=mVertices[i].m_xyz.x;
        int z=mVertices[i].m_xyz.z;
        grid[x][z].first+=mVertices[i].m_xyz.y;
        grid[x][z].second+=1;

    }

    mVertices.clear();//vårrengjøring

    //finne average høyde i hver grid point
    for(auto i=0;i<(int)max[0];i++){
        for(auto j=0;j<(int)max[2];j++){
            if(grid[i][j].second!=0)
                grid[i][j].first=grid[i][j].first/(float)grid[i][j].second;
            mVertices.push_back(Vertex(i,grid[i][j].first,j));
        }
    }

    //indeksering (med hjelp fra Hans Ola <3)
    for(auto i=0;i<(int)max[2]-1;i++){
        for(auto j=0;j<(int)max[0]-1;j++){
            int index = i*(int)max[0]+j;
            mIndices.push_back(index);
            mIndices.push_back(index+1);
            mIndices.push_back(index+1+(int)max[0]);
            mIndices.push_back(index);
            mIndices.push_back(index+1+(int)max[0]);
            mIndices.push_back(index+(int)max[0]);
     }
    }



// ut glm::vec3 grid[(int)max[0]][(int)max[2]];

    World::getWorld().surface = this;
}

void TriangleSurface::readCompressed(std::string filnavn) //brukes for å lese komprimert og optimalisert fil
{
    qDebug()<<"compressed constructor called \n";
    std::ifstream file;
    file.open("../3Dprog22/"+filnavn);
    if(file.is_open()){
        Vertex tempVert;
        int vertInt;
        int indexInt;
        std::string inText;
        file>>vertInt;
        mVertices.reserve(vertInt);

        for(int i=0;i<vertInt;i++){
            float x,y,z;
            char d1,d2,d3,d4;
            file>>d1>>x>>d2>>y>>d3>>z>>d4;
            tempVert.m_xyz={x,y,z};
            tempVert.m_normal ={x/100.f,y/100.f,x/100.f};
            tempVert.m_color=tempVert.m_normal;
            mVertices.push_back(tempVert);
        }
        file>>indexInt;
        mIndices.reserve(indexInt);
        triangleCount=indexInt/3;
        for(int i=0;i<indexInt;i++){
            int index;
            file>>index;
            mIndices.push_back(index);
        }
        if(file.eof())
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

    // 4th attribute buffer : color
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_color) );
    glEnableVertexAttribArray(3);

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
    if(triangleIndex > triangleCount || triangleIndex == -1)
        return glm::vec3 {0.f};

    glm::vec3 vPos[3];
    for(int i = 0; i < 3; i++)
    {
        vPos[i] = mVertices[mIndices[triangleIndex * 3 + i]].m_xyz;
    }
    glm::vec3 ba = vPos[1]-vPos[0];
    glm::vec3 ca = vPos[2]-vPos[0];
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

bool TriangleSurface::tooClose(Vertex a, Vertex b)      // Sjekk om gitte vertexer er for nærme
{
    Vertex temp;
    if(abs(glm::distance2(a.m_xyz,b.m_xyz))<minDist)
        return true;
    else return false;

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

