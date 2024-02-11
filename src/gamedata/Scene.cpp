#include "Scene.h"
#include "Actor.h"

// std library
#include <string>
#include <unordered_map>
#include <iostream>
#include <vector>

void Scene::instantiateActor(Actor& actor) {
	glm::dvec2 actorPos(actor.transform.pos.x, actor.transform.pos.y);
	Actor* actorPtr = &actor;
	actor.id = actors.size();
	
	// insert the actor into the list of actors
	actors.emplace_back(actor);

	// insert the (location, actors) pair into the unordered map
	locToActors[actorPos].emplace_back(actorPtr);
}