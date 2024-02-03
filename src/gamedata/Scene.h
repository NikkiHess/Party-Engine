#pragma once

// std stuff
#include <string>
#include <vector>
#include <unordered_map>

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
// this is after private this time because I want to keep KeyFuncs private (but do I need to?)
public:
	std::vector<Actor> actors;
	std::unordered_map<glm::ivec2, std::vector<Actor*>, KeyFuncs, KeyFuncs> locToActors;
	std::string name;

	void createActor(const std::string& name, const char& view,
		const int& x, const int& y, const int& velX, const int& velY,
		const bool& blocking, const std::string& nearbyDialogue,
		const std::string& contactDialogue);

	void deleteActors();

	~Scene() {
		deleteActors();
	}
};

