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
	glm::dvec2 scale; // double vec
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

class ActorProps {
public:
	std::string name = "";
	Transform transform;
	View view;
	glm::ivec2 velocity;
	bool blocking = false;
	std::string nearbyDialogue = "", contactDialogue = "";
};

struct Actor {
public:
	std::string name;
	Transform transform;
	View view;
	glm::dvec2 velocity;
	bool blocking;
	std::string nearbyDialogue;
	std::string contactDialogue;

	int id = 0;
	bool triggeredScoreUp = false;

	Actor(ActorProps& props)
		: name(props.name), transform(props.transform), view(props.view), 
		  velocity(glm::ivec2(props.velocity.x, props.velocity.x)), blocking(props.blocking), 
		  nearbyDialogue(props.nearbyDialogue), contactDialogue(props.contactDialogue) {}

	Actor() {}

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
