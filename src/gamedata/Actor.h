#pragma once

// std library
#include <string>
#include <optional>

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

	// PLAYER ONLY STATISTICS
	// should be unused otherwise
	int health = 3, score = 0;

	int id = 0;
	bool triggeredScoreUp = false;

	// prints the actor's contact dialogue
	void printContactDialogue() const;

	// prints the actor's nearby dialogue
	void printNearbyDialogue() const;
};

class ActorComparator {
public:
	bool operator()(Actor* actor1, Actor* actor2) {
		return actor1->id < actor2->id;
	}
};
