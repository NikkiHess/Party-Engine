#pragma once

// std stuff
#include <string>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

// my code
#include "Actor.h"
#include "../utils/config/RenderingConfig.h"
#include "../GameState.h"
#include "../utils/config/GameConfig.h"
#include "../audio/AudioPlayer.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL.h"

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
	std::vector<std::shared_ptr<Actor>> actors;

	std::set<Actor*, ActorComparator> actorsWithOnStart;
	std::set<Actor*, ActorComparator> actorsWithOnUpdate;
	std::set<Actor*, ActorComparator> actorsWithOnLateUpdate;

	std::set<Actor*, ActorComparator> actorsWithNewComponents;
	std::set<Actor*, ActorComparator> actorsWithComponentsToRemove;


	std::map<std::string, std::set<Actor*>> actorsByName;

	// actors sorted by render order -> y pos -> id
	std::set<Actor*, RenderOrderComparator> actorsByRenderOrder;

	std::string name;

	// instantiate an actor in the scene
	void instantiateActor(Actor& actor);
};

