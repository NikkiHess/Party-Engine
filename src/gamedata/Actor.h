#pragma once

// std library
#include <optional>
#include <string>

// my code
#include "../utils/OptionalVec2.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_render.h"

class Transform {
public:
	glm::vec2 pos = { 0.0, 0.0 };
	glm::vec2 scale = { 1.0, 1.0 };
	float rotationDegrees = 0.0;

	// whether the actor is currently flipped (attempting move west)
	bool flipped = false;
	bool showBack = false;
	bool bounce = false;
};

class TextureImage {
public:
	SDL_Texture* image = nullptr;
	std::string name = "";
	glm::ivec2 size = glm::ivec2(0);
};

class View {
public:
	TextureImage imageFront;
	TextureImage imageBack;
	// the pivot offset, in pixels
	OptionalVec2 pivot;
};

class BoxCollider {
public:
	// the size of the box collider (uses x and y)
	glm::vec2 size;

	// the extents of the box collider (relative)
	// calculated if and only if size is received
	glm::vec4 extents;

	// calculates extents given pivot and size
	void calculateExtents(OptionalVec2& pivot);
};

class Actor {
public:
	// the actor's name
	std::string name = "";
	// the actor's transform
	Transform transform;
	// the actor's view (imageFront, imageBack, and pivot)
	View view;
	// the actor's x and y velocity
	glm::vec2 velocity;
	bool movementBounce = false;

	// the actor's dialogue
	std::string nearbyDialogue = "", contactDialogue = "";

	// the actor's location in the render order
	int renderOrder = 0;

	// the actor's collider properties
	BoxCollider boxCollider;

	// PLAYER ONLY PROPERTIES!!!
	// should be unused otherwise
	int health = 3, score = 0;
	int lastHealthDownFrame = -180;
	int healthDownCooldown = 180;
	float speed = 0.02f;

	int id = 0;
	bool triggeredScoreUp = false;
    
    Actor() : velocity(0, 0) {}

	void handleFlipping(bool flipping);
	void handleVerticalFacing();
};

class ActorComparator {
public:
	bool operator()(Actor* actor1, Actor* actor2) const;
};

class RenderOrderComparator {
public:
	bool operator()(const Actor* actor1, const Actor* actor2) const;
};
