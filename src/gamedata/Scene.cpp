#include "Scene.h"

// std stuff
#include <string>
#include <unordered_map>
#include <iostream>

void Scene::createActor(const std::string& name, const char& view,
						 const int& x, const int& y, const int& velX, const int& velY,
					 	 const bool& blocking, const std::string& nearbyDialogue,
						 const std::string& contactDialogue) {
	glm::ivec2 actorPos(x, y);
	Actor* actorPtr = new Actor(name, view, actorPos, glm::ivec2(velX, velY), blocking, nearbyDialogue, contactDialogue);
	Actor &actor = *actorPtr;
	actor.id = actors.size();
	
	// insert the actor into the list of actors
	actors.push_back(actor);

	// insert the (location, actors) pair into the unordered map
	auto it = locToActors.find(actorPos);
	if (it != locToActors.end())
		it->second.push_back(actorPtr);
	else
		locToActors.insert({actorPos, {actorPtr}});
}

void Scene::deleteActors() {
	locToActors.clear();
}