#pragma once

// std stuff
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>

// my code
#include "Actor.h"

// dependencies
#include "glm/glm.hpp"

class Scene
{
private:
	// handles hashing for dvec2
	struct HashLoc {
		size_t operator()(const glm::dvec2& k) const {
			return std::hash<double>()(k.x) ^ std::hash<double>()(k.y);
		}

		bool operator()(const glm::dvec2& a, const glm::dvec2& b) const {
			return a.x == b.x && a.y == b.y;
		}
	};
public:
	std::vector<Actor> actors;
	std::set<Actor*, RenderOrderComparator> actorsByRenderOrder; // actors sorted by their render order
	std::set<Actor*, ActorComparator> motionActors;
	std::unordered_map<glm::dvec2, std::unordered_set<Actor*>, HashLoc, HashLoc> locToActors;
	std::string name;

	// instantiate an actor in the scene
	void instantiateActor(Actor& actor);

	// move all NPC actors in the scene
	void moveNPCActors();

	// move an actor according to its velocity
	// and update its render order
	void moveActor(Actor* actor);

	// check if an actor would collide given its velocity
	bool wouldCollide(Actor* actor);

	~Scene() {
		locToActors.clear();
	}
};

