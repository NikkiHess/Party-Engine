#pragma once

// std stuff
#include <string>
#include <unordered_map>
#include <vector>

// my code
#include "Actor.h"

// dependencies
#include "glm/glm.hpp"

class Scene
{
private:
	struct KeyFuncs {
		size_t operator()(const glm::ivec2& k) const {
			return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
		}

		bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
			return a.x == b.x && a.y == b.y;
		}
	};
public:
	std::vector<Actor> actors;
	std::vector<Actor*> actorsByRenderOrder;
	std::unordered_map<glm::ivec2, std::vector<Actor*>, KeyFuncs, KeyFuncs> locToActors;
	std::string name;

	// instantiate an actor in the scene
	void instantiateActor(Actor& actor);

	~Scene() {
		locToActors.clear();
	}
};

