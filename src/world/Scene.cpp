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
	actor.id = actors.size();
	
	// insert the actor into the list of actors
	actors.emplace_back(actor);

	// insert the actor into the "sorted-by-render-order" list
	actorsByRenderOrder.emplace(&actors.back());

	// if an actor has velocity, insert into motion list
	if (std::abs(actor.velocity.x) > 0 || std::abs(actor.velocity.y) > 0 || actor.name == "player") {
		motionActors.emplace(&actors.back());
	}
}

GameState Scene::moveAllActors(bool flipping, GameState& currentState, GameConfig& gameConfig, AudioPlayer& audioPlayer) {
	GameState outState = currentState;
	std::map<std::string, Actor*> dialogue;

	Actor* player = nullptr;

	for (Actor* actor : motionActors) {
		// possibly rendundant check, leaving it here just in case :)
		if (std::abs(actor->velocity.x) > 0 || std::abs(actor->velocity.y) > 0) {
			std::map<std::string, Actor*> newDialogue = moveActor(actor, flipping);
			for (auto& it : newDialogue) {
				dialogue[it.first] = it.second;
			}

			if (actor->name == "player") {
				player = actor;
				actor->velocity = glm::vec2(0);
			}
		}
	}

	return outState;
}

std::map<std::string, Actor*> Scene::moveActor(Actor* actor, bool flipping) {
	std::map<std::string, Actor*> outDialogue;

	// remove the old position of the actor from the unordered_map
	actorsByRenderOrder.erase(actor);

	// update the instanced actor's position if they wouldn't6 collide
	actor->transform.pos += actor->velocity;

	// add the updated position of the actor to the unordered_map
	actorsByRenderOrder.emplace(actor);

	return outDialogue;
}