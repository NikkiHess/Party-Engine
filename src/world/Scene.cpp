// std library
#include <memory>
#include <vector>

// my code
#include "Actor.h"
#include "Scene.h"

void Scene::instantiateActor(Actor& actor, bool doLifecycle) {
	actor.id = static_cast<int>(Actor::currentId);

	auto actorShared = std::make_shared<Actor>(actor);

	// insert the actor into the list of actors
	actors.emplace_back(actorShared);

	// insert the actor's ptr into the actors by name list
	actorsByName[actorShared->name].emplace(actorShared);
	actorsById[actorShared->id] = actorShared;

	if (doLifecycle) {
		instantiateActorLifecycle(actorShared, true);
	}

	++Actor::currentId;
}

void Scene::instantiateActorLifecycle(std::shared_ptr<Actor>& actorShared, bool doOnStart) {
	if (doOnStart) {
		if (actorShared->componentsWithOnStart.size() > 0)
			actorsWithOnStart.emplace(actorShared);
	}

	if (actorShared->componentsWithOnUpdate.size() > 0)
		actorsWithOnUpdate.emplace(actorShared);

	if (actorShared->componentsWithOnLateUpdate.size() > 0)
		actorsWithOnLateUpdate.emplace(actorShared);

	if (actorShared->componentsWithOnClick.size() > 0)
		actorsWithOnClick.emplace(actorShared);

	if (actorShared->componentsWithOnMouse.size() > 0)
		actorsWithOnMouse.emplace(actorShared);

	if (actorShared->componentsWithOnExit.size() > 0)
		actorsWithOnExit.emplace(actorShared);
}