#include "kurve.h"

kurve::kurve(): VisualObject("plainshader")
{
    for(int i =0;i<3;i++)
        skjoter.push_back(0);
    for(int i=0;i<10;i++){
        mVertices.push_back(Vertex(0,0,0));
        mVertices[i].m_normal={1,1,1};
        mVertices[i].m_color={0.f,1.f,1.f};
    }
    mNavn="LINJEBOY";
}

void kurve::init(GLint matrixUniform)
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


}

void kurve::draw(const glm::mat4 &pMat)
{
    VisualObject::draw(pMat);
    glUniformMatrix4fv( mMatrixUniform, 1, GL_FALSE, glm::value_ptr(glm::mat4(1)));
    glDrawArrays(GL_LINES, 0, (GLsizei)mVertices.size());
}

void kurve::update(glm::vec3 inPos)
{
    //qDebug()<<"called update";
    cPoints.push_back(inPos);
    skjoter.push_back(iterator);

//    for(int i=0;i<cPoints.size();i++){
//        mVertices[i]=Vertex(inPos.x,inPos.y,inPos.z);
//    }
    if(skjoter.size()>5)
        reCalc();

    qDebug()<<"control point # : "<<cPoints.size();
    qDebug()<<inPos.x<<inPos.y<<inPos.z;
}

float kurve::bFunc(int i, int d, float t)
{
    if(d==0){
        if(skjoter[i]<=t&&t<skjoter[i+1])
            return 1;
        else
            return 0;
    }

    return wFunc(i,d,t)*bFunc(i,d-1,t)+(1-wFunc(i+1,d,t))*bFunc(i+1,d-1,t);
}

float kurve::wFunc(int i, int d, float t)
{
        if(skjoter[i]<skjoter[i+d])
            return (t-skjoter[i])/(skjoter[i+d]-skjoter[i]);
        else
            return 0;
}

void kurve::reCalc()
{
    //mVertices.clear()
    if(skjoter.back()==iterator&&skjoter[skjoter.size()-2]==iterator&&skjoter[skjoter.size()-3]==iterator){
        iterator++;
        skjoter[skjoter.size()-1]=iterator;
        skjoter[skjoter.size()-2]=iterator;
        //skjoter[skjoter.size()-3]=iterator;
    }

    float stepSize=(float)skjoter.back()/(float)mVertices.size();
    for(float f=0.f;f<skjoter.back();f+=stepSize){
        glm::vec3 temp = {0,0,0};
        for(int i=0;i<cPoints.size();i++){
            temp=cPoints[i]*bFunc(i, 2, f);
            mVertices[i]=Vertex(temp.x,temp.y,temp.z);
    }

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferSubData(GL_ARRAY_BUFFER,0,mVertices.size()*sizeof(Vertex),mVertices.data());

//    qDebug() << "verts:";
//    for(int i=0;i<mVertices.size();i++){
//        qDebug()<<mVertices[i].m_xyz.x<< mVertices[i].m_xyz.y << mVertices[i].m_xyz.z;
//    }
    }
    //qDebug()<<"vertices : "<<mVertices.size();
    //qDebug() << "last entry : " << mVertices.back().m_xyz.x<< mVertices.back().m_xyz.y << mVertices.back().m_xyz.z;
}
