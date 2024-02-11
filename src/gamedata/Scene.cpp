#include "Scene.h"
#include "Actor.h"

// std library
#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

Actor& Scene::instantiateActor(ActorProps& props) {
	glm::dvec2 actorPos(props.transform.pos.x, props.transform.pos.y);
	Actor* actorPtr = new Actor(props);
	Actor &actor = *actorPtr;
	actor.id = actors.size();
	
	// insert the actor into the list of actors
	actors.emplace_back(actor);

	// insert the (location, actors) pair into the unordered map
	locToActors[actorPos].emplace_back(actorPtr);

	return actor;
}