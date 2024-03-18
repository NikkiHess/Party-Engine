// std library
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>

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
	actor.setID(static_cast<int>(actors.size()));
	
	// insert the actor into the list of actors
	actors.emplace_back(actor);

	// insert the actor into the "sorted-by-render-order" list
	actorsByRenderOrder.emplace(&actors.back());

	for (auto& [name, component] : actor.componentsWithOnStart) {
		component->onStart(component->instanceTable);
	}
}