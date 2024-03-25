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

	std::set<std::shared_ptr<Actor>, ActorComparator> actorsWithOnStart;
	std::set<std::shared_ptr<Actor>, ActorComparator> actorsWithOnUpdate;
	std::set<std::shared_ptr<Actor>, ActorComparator> actorsWithOnLateUpdate;

	std::set<std::shared_ptr<Actor>, ActorComparator> actorsWithNewComponents;
	std::set<std::shared_ptr<Actor>, ActorComparator> actorsWithComponentsToRemove;
	std::set<std::shared_ptr<Actor>, ActorComparator> actorsToAdd;
	std::set<std::shared_ptr<Actor>, ActorComparator> actorsToRemove;

	std::map<std::string, std::set<std::shared_ptr<Actor>>> actorsByName;
	std::map<int, std::shared_ptr<Actor>> actorsById;

	// actors sorted by render order -> y pos -> id
	std::set<std::shared_ptr<Actor>, RenderOrderComparator> actorsByRenderOrder;

	std::string name;

	// instantiate an actor in the scene
	// actor - the actor to be instantiated
	// doLifecycle - whether to populate lists for lua lifecycle functions
	void instantiateActor(Actor& actor, bool doPtr);

	void instantiateActorLifecycle(std::shared_ptr<Actor>& actorShared);
};

