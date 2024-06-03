// std library
#include <string>

// my code
#include "../world/physics/VectorWrapper.h"
#include "CppComponent.h"

#pragma once
class Rigidbody : public CppComponent
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float gravityScale = 1.0f;
	float density = 1.0f;
	float angularFriction = 0.3f;
	float rotation = 0.0f; // degrees
	
	/*
	dynamic: use b2_dynamicBody with b2BodyDef.type
	static: use b2_staticBody with b2BodyDef.type
	kinematic: use b2_kinematicBody with b2BodyDef.type
	*/
	std::string bodyType = "dynamic";

	bool precise = true;
	bool hasCollider = true;
	bool hasTrigger = true;

	Rigidbody() {}

	// lua OnStart
	void onStart();

	VectorWrapper getPosition();
	float getRotation();

	~Rigidbody() {}
private:
	b2Body* body = nullptr;
};