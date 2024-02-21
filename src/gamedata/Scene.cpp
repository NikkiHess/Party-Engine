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

	// all actors that can collide need to be kept track of as well
	if (actor.boxCollider.canCollide) {
		collisionActors.emplace(&actors.back());
	}

	// insert the (location, actors) pair into the unordered map
	locToActors[actorPos].emplace(&actors.back());
}

void Scene::moveNPCActors(bool flipping, ResourceManager& resourceManager) {
	for (Actor* actor : motionActors) {
		// possibly rendundant check, leaving it here just in case :)
		if (std::abs(actor->velocity.x) > 0 || std::abs(actor->velocity.y) > 0) {
			moveActor(actor, flipping, resourceManager);
		}
	}
}

void Scene::moveActor(Actor* actor, bool flipping, ResourceManager& resourceManager) {
	// NPCS: if collision, reverse velocity + move next turn
	// PLAYER: if collision, don't move
	// if no collision, keep moving
	if (!wouldCollide(actor, resourceManager)) {
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

bool Scene::wouldCollide(Actor* actor, ResourceManager& resourceManager) {
	glm::ivec2 futurePosition = actor->transform.pos + actor->velocity;

	/*Extents& acExtents = actor->boxCollider.extents;
	Extents acPhysical = {
		actor->transform.pos.y + acExtents.top.value(),
		actor->transform.pos.y + acExtents.bottom.value(),
		actor->transform.pos.x + acExtents.left.value(),
		actor->transform.pos.x + acExtents.right.value()
	};
	for (Actor* other : collisionActors) {
		if (other == actor) continue;

		Extents& ocExtents = other->boxCollider.extents;
		Extents ocPhysical = {
			other->transform.pos.y + ocExtents.top.value(),
			other->transform.pos.y + ocExtents.bottom.value(),
			other->transform.pos.x + ocExtents.left.value(),
			other->transform.pos.x + ocExtents.right.value()
		};

		if (acPhysical.right > ocPhysical.left && acPhysical.left < ocPhysical.right) {
			if (acPhysical.bottom > ocPhysical.top && acPhysical.top < ocPhysical.bottom) {
				std::cout << "coll " << actor->name << " and " << other->name << "\n";
				return true;
			}
		}
	}*/

	return false;
}