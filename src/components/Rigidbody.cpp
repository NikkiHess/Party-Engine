// my code
#include "../GameEngine.h"
#include "Rigidbody.h"

// box2d
#include "box2d/box2d.h"

// on start, make sure the engine knows we need b2World
// and initialize some stuff
void Rigidbody::onStart() {
	if (!Engine::needsb2World) {
		Engine::needsb2World = true;
	}

	b2BodyDef bodyDef;

	// set type
	if (bodyType == "dynamic") bodyDef.type = b2_dynamicBody;
	else if (bodyType == "static") bodyDef.type = b2_staticBody;
	else if (bodyType == "kinematic") bodyDef.type = b2_kinematicBody;

	bodyDef.bullet = precise;
	bodyDef.angularDamping = angularFriction;
	bodyDef.gravityScale = gravityScale;

	// create our body from our body def
	body = Engine::world->CreateBody(&bodyDef);
	
	// define a basic fixture
	b2PolygonShape shape;
	shape.SetAsBox(0.5f, 0.5f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.density = 1.0f;

	body->CreateFixture(&fixtureDef);
}

// returns the position as a vector wrapper
VectorWrapper Rigidbody::getPosition() {
	return VectorWrapper(body->GetPosition().x, body->GetPosition().y);
}

// returns the rotation (in radians)
float Rigidbody::getRotation() {
	float angleDeg = body->GetAngle();
	float angleRad = angleDeg * (b2_pi / 180.0f);

	return angleRad;
}