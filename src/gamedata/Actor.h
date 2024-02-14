#pragma once

// std library
#include <optional>
#include <string>

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_render.h"

class Transform {
public:
	glm::dvec2 pos; // double vec
	glm::dvec2 scale = { 1.0, 1.0 }; // double vec
	double rotationDegrees = 0.0;
};

class OptionalDvec2 {
public:
	std::optional<double> x, y;
};

class View {
public:
	std::string imageName = "";
	SDL_Texture* image = nullptr;
	OptionalDvec2 pivotOffset;
};

class Actor {
public:
	std::string name = "";
	Transform transform;
	View view;
	glm::dvec2 velocity;
	bool blocking = false;
	std::string nearbyDialogue = "", contactDialogue = "";
	int renderOrder = 0;

	// PLAYER ONLY STATISTICS
	// should be unused otherwise
	int health = 3, score = 0;
	int lastHealthDownFrame = -180;
	int healthDownCooldown = 180;

	int id = 0;
	bool triggeredScoreUp = false;
    
    Actor() : velocity(0, 0) {}
};

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
