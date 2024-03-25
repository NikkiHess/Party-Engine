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

void Scene::instantiateActor(Actor& actor) {
	glm::vec2 actorPos(actor.transform.pos.x, actor.transform.pos.y);
	actor.id = static_cast<int>(actors.size());

	auto actorShared = std::make_shared<Actor>(actor);

	// insert the actor into the list of actors
	actors.emplace_back(actorShared);

	Actor* ptr = &*actorShared;

	// insert the actor's ptr into the actors by name list
	actorsByName[actor.name].emplace(ptr);

	if (actor.hasOnStart)
		actorsWithOnStart.emplace(ptr);
	if (actor.hasOnUpdate)
		actorsWithOnUpdate.emplace(ptr);
	if (actor.hasOnLateUpdate)
		actorsWithOnLateUpdate.emplace(ptr);

	// insert the actor's ptr into the "sorted-by-render-order" list
	actorsByRenderOrder.emplace(ptr);
}