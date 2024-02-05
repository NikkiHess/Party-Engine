#include "Scene.h"
#include "Actor.h"

// std library
#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

Actor& Scene::instantiateActor(ActorProps& props) {
	glm::ivec2 actorPos(props.x, props.y);
	Actor* actorPtr = new Actor(props);
	Actor &actor = *actorPtr;
	actor.id = actors.size();
	
	// insert the actor into the list of actors
	actors.emplace_back(actor);

	// insert the (location, actors) pair into the unordered map
	locToActors[actorPos].emplace_back(actorPtr);

	return actor;
}

void Scene::deleteActors() {
	locToActors.clear();
}