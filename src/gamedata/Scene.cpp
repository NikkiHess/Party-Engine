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
	actorsByRenderOrder.emplace_back(&actors.back());

	// insert the (location, actors) pair into the unordered map
	locToActors[actorPos].emplace_back(&actors.back());
}