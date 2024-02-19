#pragma once

// std library
#include <optional>
#include <string>

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_render.h"

class Transform {
public:
	glm::vec2 pos = { 0.0, 0.0 };
	glm::vec2 scale = { 1.0, 1.0 };
	float rotationDegrees = 0.0;
};

class OptionalVec2 {
public:
	std::optional<float> x, y;
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
	bool blocking = false;
	std::string nearbyDialogue = "", contactDialogue = "";
	int renderOrder = 0;

	// whether the actor is currently flipped (attempting move west)
	bool flipped = false;
	bool showBack = false;

	// PLAYER ONLY PROPERTIES!!!
	// should be unused otherwise
	int health = 3, score = 0;
	int lastHealthDownFrame = -180;
	int healthDownCooldown = 180;
	float speed = 0.02f;

	int id = 0;
	bool triggeredScoreUp = false;
    
    Actor() : velocity(0, 0) {}
};

// TODO: Move to .cpp, this is ugly
class ActorComparator {
public:
	bool operator()(Actor* actor1, Actor* actor2) const {
		return actor1->id < actor2->id;
	}
};

class RenderOrderComparator {
public:
	bool operator()(const Actor* actor1, const Actor* actor2) const {
		// render orders equal? render by y pos
		if (actor1->renderOrder == actor2->renderOrder) {
			// y pos equal? render by id
			if (actor1->transform.pos.y == actor2->transform.pos.y) {
				return actor1->id < actor2->id;
			}

			// y pos not equal? compare
			return actor1->transform.pos.y < actor2->transform.pos.y;
		}
		// render order not equal? compare
		return actor1->renderOrder < actor2->renderOrder;
	}
};
