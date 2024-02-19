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
	OptionalVec2 pivotOffset;
};

class Actor {
public:
	std::string name = "";
	Transform transform;
	View view;
	glm::vec2 velocity;
	bool movementBounce = false;
	bool blocking = false;
	std::string nearbyDialogue = "", contactDialogue = "";
	int renderOrder = 0;

	// whether the actor is currently flipped (attempting move west)
	bool flipped = false;
	bool showBack = false;
	bool bounce = false;

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
