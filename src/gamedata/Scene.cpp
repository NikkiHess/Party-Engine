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
			moveActor(actor, flipping);
		}
	}
}

void Scene::moveActor(Actor* actor, bool flipping) {
	// NPCS: if collision, reverse velocity + move next turn
	// PLAYER: if collision, don't move
	// if no collision, keep moving
	if (!wouldCollide(actor)) {
		// remove the old position of the actor from the unordered_map
		locToActors[actor->transform.pos].erase(actor);
		actorsByRenderOrder.erase(actor);

		// update the instanced actor's position if they wouldn't collide
		actor->transform.pos += actor->velocity;

		actor->handleFlipping(flipping);
		actor->handleVerticalFacing();

		if (actor->movementBounce) {
			actor->transform.bounce = true;
		}

		// add the updated position of the actor to the unordered_map
		locToActors[actor->transform.pos].emplace(actor);
		actorsByRenderOrder.emplace(actor);
	}
	else if (actor->name != "player") {
		actor->velocity = -actor->velocity;
	}
}

bool Scene::wouldCollide(Actor* actor) {
	glm::ivec2 futurePosition = actor->transform.pos + actor->velocity;

	// if the actor can collide and the boxCollider has no extents, configure them
	if (actor->boxCollider.canCollide && !actor->boxCollider.hasExtents()) {
		actor->boxCollider.calculateExtents(actor->view.pivot, actor->view.imageFront.size);
	}

	return false;
}