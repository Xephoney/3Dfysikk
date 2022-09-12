#include "cubemap.h"
#include <glm/gtc/type_ptr.hpp>
#include <src/vendor/stb_image/stb_image.h>
#include "shadercoordinator.h"

Cubemap::Cubemap()
{

}

Cubemap::~Cubemap()
{

}

void Cubemap::init()
{
    ShaderCoordinator& SC = ShaderCoordinator::getInstance();
    mShader = SC.fetchSkyboxShader();
    mShader->use();

    initializeOpenGLFunctions();
    CreateBox();

    //Vertex Array Object
    glGenVertexArrays( 1, &mVAO );
    glBindVertexArray( mVAO );

    //Vertex Buffer Object
    glGenBuffers( 1, &mVBO );
    glBindBuffer( GL_ARRAY_BUFFER, mVBO );
    glBufferData( GL_ARRAY_BUFFER, mVertices.size()*sizeof( Vertex ), mVertices.data(), GL_STATIC_DRAW );

    //Index buffer
    glGenBuffers( 1, &mIBO );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIBO );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, mIndices.size()*sizeof(unsigned int), mIndices.data(), GL_STATIC_DRAW );

    // Attribute buffer : Position
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)NULL);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

//    std::string cubemapTex[6] {
//        "../3Dprog22/assets/textures/cubemap/skybox_a_right.jpg",
//        "../3Dprog22/assets/textures/cubemap/skybox_a_left.jpg",
//        "../3Dprog22/assets/textures/cubemap/skybox_a_top.jpg",
//        "../3Dprog22/assets/textures/cubemap/skybox_a_bottom.jpg",
//        "../3Dprog22/assets/textures/cubemap/skybox_a_front.jpg",
//        "../3Dprog22/assets/textures/cubemap/skybox_a_back.jpg"
//    };
    std::string cubemapTex[6] {
        "../3Dprog22/assets/textures/cubemap/right.png",
        "../3Dprog22/assets/textures/cubemap/left.png",
        "../3Dprog22/assets/textures/cubemap/top.png",
        "../3Dprog22/assets/textures/cubemap/bottom.png",
        "../3Dprog22/assets/textures/cubemap/front.png",
        "../3Dprog22/assets/textures/cubemap/back.png"
    };


    glGenTextures(1, &cubemapTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for(unsigned int i = 0; i < 6; i++)
    {
        int width {0};
        int height {0};
        int channels;

        unsigned char* data = stbi_load(cubemapTex[i].c_str(), &width, &height, &channels, 0);
        if( data )
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D
            (
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data
            );
        }
        else
        {
            qDebug() << "CUBEMAP | Failed to load texture at path " << cubemapTex[i].c_str();
        }

        stbi_image_free(data);
    }

}

void Cubemap::draw(const glm::mat4 &projMatrix, const glm::mat4 &viewMatrix)
{
    initializeOpenGLFunctions();
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_DEPTH_TEST);
    mShader->use();
    glBindVertexArray(mVAO);

    glm::mat4 view = glm::mat4(glm::mat3(viewMatrix));
    mShader->SetUniformMatrix4fv("k_view", view);
    mShader->SetUniformMatrix4fv("k_projection", projMatrix);
    mShader->SetUniform1i("skybox", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID);
    glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    //glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
}

void Cubemap::loadTextures(std::string Path)
{

}

void Cubemap::CreateBox()
{
    mVertices.push_back(Vertex { -1.f, -1.f,  1.f }   );
    mVertices.push_back(Vertex {  1.f, -1.f,  1.f }   );
    mVertices.push_back(Vertex {  1.f, -1.f, -1.f }   );
    mVertices.push_back(Vertex { -1.f, -1.f, -1.f }   );
    mVertices.push_back(Vertex { -1.f,  1.f,  1.f }   );
    mVertices.push_back(Vertex {  1.f,  1.f,  1.f }   );
    mVertices.push_back(Vertex {  1.f,  1.f, -1.f }   );
    mVertices.push_back(Vertex { -1.f,  1.f, -1.f }   );

    //right
    mIndices.push_back(1);
    mIndices.push_back(2);
    mIndices.push_back(6);
    mIndices.push_back(6);
    mIndices.push_back(5);
    mIndices.push_back(1);

    //left
    mIndices.push_back(0);
    mIndices.push_back(4);
    mIndices.push_back(7);
    mIndices.push_back(7);
    mIndices.push_back(3);
    mIndices.push_back(0);

    //top
    mIndices.push_back(4);
    mIndices.push_back(5);
    mIndices.push_back(6);
    mIndices.push_back(6);
    mIndices.push_back(7);
    mIndices.push_back(4);

    //bottom
    mIndices.push_back(0);
    mIndices.push_back(3);
    mIndices.push_back(2);
    mIndices.push_back(2);
    mIndices.push_back(1);
    mIndices.push_back(0);

    //back
    mIndices.push_back(0);
    mIndices.push_back(1);
    mIndices.push_back(5);
    mIndices.push_back(5);
    mIndices.push_back(4);
    mIndices.push_back(0);

    //front
    mIndices.push_back(3);
    mIndices.push_back(7);
    mIndices.push_back(6);
    mIndices.push_back(6);
    mIndices.push_back(2);
    mIndices.push_back(3);
}
