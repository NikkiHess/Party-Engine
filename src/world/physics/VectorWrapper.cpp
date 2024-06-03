// my code
#include "VectorWrapper.h"

// glm
#include "glm/glm.hpp"

// Box2D
#include "box2d/box2d.h"

float VectorWrapper::getX() const {
    return glmVec.x;
}

void VectorWrapper::setX(float x) {
    glmVec.x = x;
    box2dVec.x = x;
}

float VectorWrapper::getY() const {
    return glmVec.y;
}

void VectorWrapper::setY(float y) {
    glmVec.y = y;
    box2dVec.y = y;
}

void VectorWrapper::normalize() { 
    glmVec = glm::normalize(glmVec); box2dVec.Normalize();
}

float VectorWrapper::length() const { 
    return glm::length(glmVec);
}

VectorWrapper VectorWrapper::operator+(const VectorWrapper& other) const {
    return VectorWrapper(glmVec.x + other.glmVec.x, glmVec.y + other.glmVec.y);
}

VectorWrapper VectorWrapper::operator-(const VectorWrapper& other) const {
    return VectorWrapper(glmVec.x - other.glmVec.x, glmVec.y - other.glmVec.y);
}

VectorWrapper VectorWrapper::operator*(float scalar) const {
    return VectorWrapper(glmVec.x * scalar, glmVec.y * scalar);
}