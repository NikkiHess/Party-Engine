// std library
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <memory>

// my code
#include "../GameState.h"
#include "Actor.h"
#include "Scene.h"
#include "../utils/StringUtils.h"
#include "../errors/Error.h"
#include "../utils/ResourceManager.h"

// dependencies
#include "Helper.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_rect.h"

void Scene::instantiateActor(Actor& actor, bool doLifecycle) {
	actor.id = static_cast<int>(Actor::currentId);

	auto actorShared = std::make_shared<Actor>(actor);

	// insert the actor into the list of actors
	actors.emplace_back(actorShared);

	// insert the actor's ptr into the actors by name list
	actorsByName[actorShared->name].emplace(actorShared);
	actorsById[actorShared->id] = actorShared;

	if (doLifecycle) {
		instantiateActorLifecycle(actorShared);
	}

	++Actor::currentId;
}

void Scene::instantiateActorLifecycle(std::shared_ptr<Actor>& actorShared) {
	if (actorShared->hasOnStart)
		actorsWithOnStart.emplace(actorShared);
	if (actorShared->hasOnUpdate)
		actorsWithOnUpdate.emplace(actorShared);
	if (actorShared->hasOnLateUpdate)
		actorsWithOnLateUpdate.emplace(actorShared);
	if (actorShared->hasOnExit)
		actorsWithOnExit.emplace(actorShared);
	if (actorShared->hasOnClick)
		actorsWithOnClick.emplace(actorShared);
}