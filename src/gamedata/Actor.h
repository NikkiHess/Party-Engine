#pragma once

#include <string>

#include "glm/glm.hpp"

class ActorProps {
public:
	std::string name = "";
	char view = '?';
	int x = 0, y = 0, velX = 0, velY = 0;
	bool blocking = false;
	std::string nearbyDialogue = "", contactDialogue = "";
};

struct Actor {
public:
	std::string name;
	char view;
	glm::ivec2 position;
	glm::ivec2 velocity;
	bool blocking;
	std::string nearbyDialogue;
	std::string contactDialogue;

	int id = 0;
	bool triggeredScoreUp = false;

	Actor(ActorProps& props)
		: name(props.name), view(props.view), position(glm::ivec2(props.x, props.y)), velocity(glm::ivec2(props.velX, props.velY)), 
		  blocking(props.blocking), nearbyDialogue(props.nearbyDialogue), contactDialogue(props.contactDialogue) {}

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
