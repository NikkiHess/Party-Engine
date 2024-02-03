#pragma once

#include <string>

#include "glm/glm.hpp"

struct Actor {
public:
	std::string actorName;
	char view;
	glm::ivec2 position;
	glm::ivec2 velocity;
	bool blocking;
	std::string nearbyDialogue;
	std::string contactDialogue;

	int id = 0;
	bool triggeredScoreUp = false;

	Actor(std::string actorName, char view, glm::ivec2 position, glm::ivec2 initialVelocity,
		bool blocking, std::string nearbyDialogue, std::string contactDialogue)
		: actorName(actorName), view(view), position(position), velocity(initialVelocity), blocking(blocking), nearbyDialogue(nearbyDialogue), contactDialogue(contactDialogue) {}

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