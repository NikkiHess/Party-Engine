#include "Actor.h"
#include "Scene.h"

// std library
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

void Scene::instantiateActor(Actor& actor) {
	glm::vec2 actorPos(actor.transform.pos.x, actor.transform.pos.y);
	actor.id = actors.size();
	
	// insert the actor into the list of actors
	actors.emplace_back(actor);

	// insert the actor into the "sorted-by-render-order" list
	actorsByRenderOrder.emplace(&actors.back());

	// if a non-player actor has velocity, insert into motion list
	if (actor.name != "player") {
		if (std::abs(actor.velocity.x) > 0 || std::abs(actor.velocity.y) > 0) {
			motionActors.emplace(&actors.back());
		}
	}

	// insert the (location, actors) pair into the unordered map
	locToActors[actorPos].emplace(&actors.back());
}

void Scene::moveNPCActors() {
	for (Actor* actor : motionActors) {
		if (std::abs(actor->velocity.x) > 0 || std::abs(actor->velocity.y) > 0) {
			// if no collision, keep moving
			// if collision, reverse velocity (move next turn)
			// make sure they're moving so we don't do unnecessary calculations
			// if no collision, keep moving
			if (!wouldCollide(actor)) {
				moveActor(actor);
			}
			else {
				actor->velocity = -actor->velocity;
			}
		}
	}
}

void Scene::moveActor(Actor* actor) {
	// remove the old position of the actor from the unordered_map
	locToActors[actor->transform.pos].erase(actor);
	actorsByRenderOrder.erase(actor);

	// update the instanced actor's positon
	actor->transform.pos += actor->velocity;

	// add the updated position of the actor to the unordered_map
	locToActors[actor->transform.pos].emplace(actor);
	actorsByRenderOrder.emplace(actor);
}

bool Scene::wouldCollide(Actor* actor) {
	glm::ivec2 futurePosition = actor->transform.pos + actor->velocity;

	auto it = locToActors.find(futurePosition);
	if (it != locToActors.end()) {
		for (Actor* other : it->second) {
			if (other->blocking)
				return true;
		}
	}

	return false;
}