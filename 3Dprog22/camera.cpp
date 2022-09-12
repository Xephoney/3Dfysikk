#include "camera.h"

#include "mesh.h"
#include "toolbox.h"
#include "shadercoordinator.h"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/vector_angle.hpp"

glm::mat4 Camera::getTransform()
{
    return createTransformMatrix(mPosition, mRotation, {1,1,1});
}

Camera::Camera()
{
    mPosition=startPos;
}

void Camera::init(GLint pMatrixUniform, GLint vMatrixUniform)
{
    mPmatrixUniform = pMatrixUniform;
    mVmatrixUniform = vMatrixUniform;
}

void Camera::perspective(double degrees, double aspect, double nearplane, double farplane)
{
    mPPmatrix = glm::perspective(glm::radians(degrees), aspect, nearplane, farplane);
    mPOmatrix = glm::ortho(-10.0*aspect,10.0*aspect,-10.0,10.0,nearplane, farplane);
    if(bPerspective)
        mPmatrix = mPPmatrix;
    else
        mPmatrix = mPOmatrix;
}

void Camera::lookAt(const glm::vec3 &at)
{ 
    mDirection = glm::normalize(at - mPosition);
}

void Camera::lookAt(const glm::vec3 &eye, const glm::vec3 &at)
{
    mPosition = eye;
    mDirection = glm::normalize(at - mPosition);
}

void Camera::LookAround(glm::vec2 delta)
{
    glm::vec3 newDir = glm::rotate( mDirection, glm::radians((float)(-delta.y * cameraSensitivity)), right());
    if( !(glm::angle(newDir, {0,1,0}) <= glm::radians(5.f) or glm::angle(newDir, {0,-1,0}) <= glm::radians(5.f)) )
    {
        mDirection = newDir;
    }

    mDirection = glm::rotate(mDirection, glm::radians((float)(-delta.x * cameraSensitivity)), {0, 1, 0});
}

void Camera::update()
{
    initializeOpenGLFunctions();

    mVmatrix = glm::lookAt(mPosition, mPosition + mDirection, {0,1,0});
    ShaderCoordinator::getInstance().CameraUpdate(mVmatrix, mPmatrix);
}


void Camera::tick(float dt)
{

    if(glm::length(movementVector) > 0)
    {
        mPosition += movementVector * dt * movementSpeed;
    }

    time+=dt;
    movementVector = glm::vec3(0);
}

void Camera::follow()
{
    mPosition = followPosition;
    mDirection = followViewDirection;
}


void Camera::cameraResize(double degrees, double aspect, double nearplane, double farplane)
{
    mPPmatrix = glm::perspective(glm::radians(degrees), aspect, nearplane, farplane);
    mPOmatrix = glm::ortho(-10.0*aspect,10.0*aspect,-10.0,10.0,nearplane, farplane);
    if(bPerspective)
    {
        mPmatrix = mPPmatrix;
        return;
    }
    mPmatrix = mPOmatrix;
}

void Camera::TopDown(bool check)
{
    //Sets perspective bool to be the opposite of topDown view
    bPerspective = !check;
    if(check)
    {
        mPmatrix = mPOmatrix;
        setPosition({0,100,0});
        lookAt({0,0,0},{0,0,1});
    }
    else
    {
        mPmatrix = mPPmatrix;
        setPosition({0,8,10});
        lookAt({0,0,0},{0,1,0});
    }
}

void Camera::Reset()
{
    mPosition = startPos;
    lookAt({0,0,0});
}

void Camera::rotate(float angle, glm::vec3 axis)
{
    mRotation += axis*angle;
    mVmatrix = glm::rotate(mVmatrix,glm::radians(angle),axis);
}

glm::vec3 Camera::getViewPosition()
{
    return mPosition;
}

glm::vec3 Camera::forward()
{
    return mDirection;
}

glm::vec3 Camera::right()
{
    return glm::normalize(glm::cross(mDirection, {0,1,0}));
}

glm::vec3 Camera::up()
{
    return glm::normalize(glm::cross(right(), mDirection));
}
