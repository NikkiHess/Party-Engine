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

void Scene::moveNPCActors(bool flipping) {
	for (Actor* actor : motionActors) {
		if (std::abs(actor->velocity.x) > 0 || std::abs(actor->velocity.y) > 0) {
			// if no collision, keep moving
			// if collision, reverse velocity (move next turn)
			// make sure they're moving so we don't do unnecessary calculations
			// if no collision, keep moving
			if (!wouldCollide(actor)) {
				moveActor(actor, flipping);
			}
			else {
				actor->velocity = -actor->velocity;
			}
		}
	}
}

void Scene::moveActor(Actor* actor, bool flipping) {
	// remove the old position of the actor from the unordered_map
	locToActors[actor->transform.pos].erase(actor);
	actorsByRenderOrder.erase(actor);

	// update the instanced actor's positon
	actor->transform.pos += actor->velocity;

	// if we have flipping enabled + move west, flip actor
	// if we have flipping enabled + move east, unflip actor
	// do not handle 0 velocity, keep flip the same in that case
	if (flipping) {
		if (actor->velocity.x < 0) {
			actor->flipped = true;
		}
		else if (actor->velocity.x > 0) {
			actor->flipped = false;
		}
	}

	// if we have a view_image_back
	if (actor->view.imageBack.image) {
		// velocity y is NEGATIVE if you go up
		if (actor->velocity.y < 0) {
			actor->showBack = true;
		}
		if (actor->velocity.y > 0) {
			actor->showBack = false;
		}
	}

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