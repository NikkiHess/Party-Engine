// std library
#include <iostream>

// glm
#include "glm/glm.hpp"

// Box2D
#include "box2d/box2d.h"

#pragma once
class VectorWrapper
{
public:
	glm::vec2 glmVec;
	b2Vec2 box2dVec;

	VectorWrapper() : glmVec(0.0f, 0.0f), box2dVec(0.0f, 0.0f) {}
	VectorWrapper(float x, float y) : glmVec(x, y), box2dVec(x, y) {}

	float getX() const;
	void setX(float x);

	float getY() const;
	void setY(float y);

	void normalize();
	float length() const;

	VectorWrapper operator+(const VectorWrapper& other) const;
	VectorWrapper operator-(const VectorWrapper& other) const;
	VectorWrapper operator*(float scalar) const;

	// not part of the class but can access its data
	friend std::ostream& operator<<(std::ostream& os, const VectorWrapper& vec) {
		os << "(" << vec.getX() << ", " << vec.getY() << ")";
	}
};
