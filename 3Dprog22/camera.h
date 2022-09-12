#pragma once

#include <QOpenGLFunctions_4_1_Core>
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>


class Camera : public QOpenGLFunctions_4_1_Core
{
    private:
        GLint  mPmatrixUniform;        //OpenGL reference to the Uniform in the shader program
        GLint  mVmatrixUniform;        //OpenGL reference to the Uniform in the shader program
        glm::mat4 mPPmatrix{1.f};
        glm::mat4 mPOmatrix{1.f};

        glm::mat4 mTransform{1.f};
        glm::vec3 mPosition{0,0,0};
        glm::vec3 mRotation{0,0,0};
        glm::vec3 mDirection{0,0,-1};
        glm::mat4 getTransform();
        glm::vec3 mForward{0,0,-1};
        bool bPerspective {true};
        float time = 0;
        float movementSpeed = 25.f;

        glm::vec3 startPos {40.f,16.f,40.f};
        float playerViewZ {20};
        float playerViewY {10};

        glm::vec3 playerLookAtOffset {0,5,0};
        glm::vec3 followPosition {0.f};
        glm::vec3 followViewDirection {0.f};
    public:
        Camera();
        ~Camera() { }
        void init(GLint pMatrixUniform, GLint vMatrixUniform);
        void perspective(double degrees, double aspect, double nearplane, double farplane);
        void lookAt(const glm::vec3& at);
        void lookAt(const glm::vec3& eye, const glm::vec3& at);
        void LookAround(glm::vec2 delta);
        void update();
        void tick(float dt);
        void follow();

        void cameraResize(double degrees, double aspect, double nearplane, double farplane);
        void TopDown(bool);
        void Reset();
        void rotate(float angle, glm::vec3 axis) ;
        void setPosition(glm::vec3 p){mPosition = p; } ;
        void setRotation(glm::vec3 r){mRotation = r; } ;
        glm::vec3 getPosition(){ return mPosition; } ;

        glm::vec3 getViewPosition();
        glm::vec3 forward();
        glm::vec3 right();
        glm::vec3 up();

        glm::vec3 movementVector {0,0,0};

        glm::mat4 mPmatrix{1.f};
        glm::mat4 mVmatrix{1.f};

        float cameraSensitivity = 0.1f;
};

