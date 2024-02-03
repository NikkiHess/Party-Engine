#pragma once

#include <string>

#include "glm/glm.hpp"

struct Actor {
public:
	std::string actor_name;
	char view;
	glm::ivec2 position;
	glm::ivec2 velocity;
	bool blocking;
	std::string nearby_dialogue;
	std::string contact_dialogue;
	int id = 0;

	Actor(std::string actor_name, char view, glm::ivec2 position, glm::ivec2 initial_velocity,
		bool blocking, std::string nearby_dialogue, std::string contact_dialogue)
		: actor_name(actor_name), view(view), position(position), velocity(initial_velocity), blocking(blocking), nearby_dialogue(nearby_dialogue), contact_dialogue(contact_dialogue) {}

	Actor() {}
};

class ActorComparator {
public:
	bool operator()(Actor* actor1, Actor* actor2) {
		return actor1->id < actor2->id;
	}
};