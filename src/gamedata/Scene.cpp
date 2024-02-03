#include "Scene.h"
#include "Actor.h"

// std stuff
#include <string>
#include <unordered_map>
#include <iostream>

Actor& Scene::instantiateActor(ActorProps& props) {
	glm::ivec2 actorPos(props.x, props.y);
	Actor* actorPtr = new Actor(props);
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

	return actor;
}

void Scene::deleteActors() {
	locToActors.clear();
}