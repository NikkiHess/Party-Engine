#pragma once

// std stuff
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <map>

// my code
#include "Actor.h"
#include "../utils/config/RenderingConfig.h"
#include "../gamedata/GameState.h"

// dependencies
#include "glm/glm.hpp"

class Scene
{
private:
	// handles hashing for vec2
	struct HashLoc {
		size_t operator()(const glm::vec2& k) const {
			return std::hash<float>()(k.x) ^ std::hash<float>()(k.y);
		}

		bool operator()(const glm::vec2& a, const glm::vec2& b) const {
			return a.x == b.x && a.y == b.y;
		}
	};
public:
	std::vector<Actor> actors;
	// actors sorted by render order -> y pos -> id
	std::set<Actor*, RenderOrderComparator> actorsByRenderOrder;
	// actors that move, sorted by id
	std::set<Actor*, ActorComparator> motionActors;
	// actors that collide, sorted by id
	std::set<Actor*, ActorComparator> collisionActors;
	// location to actors map, actors at location sorted by id
	std::unordered_map<glm::vec2, std::set<Actor*, ActorComparator>, HashLoc, HashLoc> locToActors;
	std::string name;

	// instantiate an actor in the scene
	void instantiateActor(Actor& actor);

	// move all NPC actors in the scene
	GameState moveAllActors(bool flipping, GameState& currentState);

	// move an actor according to its velocity
	// and update its render order
	std::map<std::string, Actor*> moveActor(Actor* actor, bool flipping);

	// check if an actor would collide if it moved (given velocity)
	void checkCollisions(Actor* actor);

	// execute commands from the given dialogue
	GameState executeCommands(Actor* player, Actor* trigger, const std::string& dialogue, int& health, GameState& currentState);

	~Scene() {
		locToActors.clear();
	}
};

