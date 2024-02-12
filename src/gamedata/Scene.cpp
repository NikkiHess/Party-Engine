#include "Actor.h"
#include "Scene.h"

// std library
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

void Scene::instantiateActor(Actor& actor) {
	glm::dvec2 actorPos(actor.transform.pos.x, actor.transform.pos.y);
	actor.id = actors.size();
	
	// insert the actor into the list of actors
	actors.emplace_back(actor);

	// insert the actor into the "sorted-by-render-order" list
	actorsByRenderOrder.emplace(&actors.back());

	// insert the (location, actors) pair into the unordered map
	locToActors[actorPos].emplace_back(&actors.back());
}

void Scene::moveActor(Actor* actor, const glm::ivec2& newPos) {
	// erase the actor from the set
	actorsByRenderOrder.erase(actor);

	// update their position
	actor->transform.pos = newPos;

	// reinsert with updated information
	actorsByRenderOrder.insert(actor);
}