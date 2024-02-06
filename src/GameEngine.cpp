// std libraries
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

// include my code
#include "GameEngine.h"
#include "utils/ConfigUtils.h"

// dependencies
#include "rapidjson/document.h"
#include "glm/glm.hpp"

// SDL2
#include "SDL2/SDL.h"
#include "SDL2/SDL_render.h"
#include "Helper.h"

// ---------- BEGIN MOTION FUNCTIONS ----------

void Engine::updatePositions() {
	for (Actor& actor : gameInfo.currentScene.actors) {
		if (actor.velocity != glm::ivec2(0)) {
			// if this is our player Actor, perform our player actor movement
			if (actor.name == "player") {
				if (!wouldCollide(actor)) {
					updateActorPosition(actor);
				}
				// stop the player's movement so they only move by 1
				actor.velocity = glm::ivec2(0, 0);
			}
			// move NPC Actors
			// if no collision, keep moving
			// if collision, reverse velocity (move next turn)
			else {
				// make sure they're moving so we don't do unnecessary calculations
				if (!wouldCollide(actor))
					updateActorPosition(actor);
				else
					actor.velocity = -actor.velocity;
			}
		}
	}
}

void Engine::updateActorPosition(Actor& actor) {
	// remove the old position of the actor from the unordered_map
	auto& locToActors = gameInfo.currentScene.locToActors;
	auto oldLocIt = locToActors.find(actor.position);
	if (oldLocIt != locToActors.end()) {
		auto& actorsAtPos = oldLocIt->second;
		actorsAtPos.erase(std::remove_if(actorsAtPos.begin(), actorsAtPos.end(),
			[&actor](const Actor* a) {
				return a->id == actor.id;
			}), actorsAtPos.end());
		if (actorsAtPos.empty()) {
			locToActors.erase(actor.position);
		}
	}

	// update the instanced actor's positon
	actor.position += actor.velocity;

	// add the new position of the actor to the unordered_map
	auto& actorsAtPos = locToActors[actor.position];
	actorsAtPos.emplace_back(&actor);
}

// check if an Actor would collide given its velocity
bool Engine::wouldCollide(Actor& actor) {
	glm::ivec2 futurePosition = actor.position + actor.velocity;

	auto it = gameInfo.currentScene.locToActors.find(futurePosition);
	if (it != gameInfo.currentScene.locToActors.end()) {
		for (Actor* other : it->second) {
			if (other->blocking)
				return true;
		}
	}

	return false;
}

// ----------- END MOTION FUNCTIONS -----------

// ----------- BEGIN CORE FUNCTIONS -----------

void Engine::handleState() {
	// Should the Engine class handle printing these messages, or should the Renderer?
	switch (gameInfo.state) {
	case WIN:
		if (gameInfo.gameOverGoodMessage != "")
			std::cout << gameInfo.gameOverGoodMessage;
		stop();
		break;
	case LOSE:
		if (gameInfo.gameOverBadMessage != "")
			std::cout << gameInfo.gameOverBadMessage;
		stop();
		break;
	default:
		break;
	}
}

void Engine::start() {
	// SDL code
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
		return;
	}

	// a window with proprties as defined by configUtils
	SDL_Window* window = SDL_CreateWindow(
		configUtils.gameTitle.c_str(),	// window title
		SDL_WINDOWPOS_CENTERED,			// initial x
		SDL_WINDOWPOS_CENTERED,			// iniital y
		configUtils.renderSize.x,		// width, in pixels
		configUtils.renderSize.y,		// height, in pixels
		SDL_WINDOW_SHOWN				// flags
	);

	// if our window couldn't be created, we have a problem
	if (window == nullptr) {
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
		SDL_Quit();
		return;
	}

	// Create our Renderer using our window, -1 (go find a display), and VSYNC/GPU rendering enabled
	SDL_Renderer* sdlRenderer = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

	//// print the starting message
	//if (gameInfo.gameStartMessage != "")
	//	std::cout << gameInfo.gameStartMessage << "\n";

	isGameRunning = true;

	while (isGameRunning) {
		// Process events
		SDL_Event nextEvent;
		while (Helper::SDL_PollEvent498(&nextEvent)) {
			if (nextEvent.type == SDL_QUIT) {
				exit(0);
			}
		}

		// Clear the frame buffer at the beginning of a frame
		SDL_SetRenderDrawColor(sdlRenderer, configUtils.clearColor.r, configUtils.clearColor.g, configUtils.clearColor.b, 1);
		SDL_RenderClear(sdlRenderer);

		Helper::SDL_RenderPresent498(sdlRenderer);

		//// print the initial render of the world
		//renderer.render(gameInfo);

		//renderer.printDialogue(gameInfo);
		//handleState();

		//// prompt the player to take an action
		//renderer.promptPlayer(gameInfo);
		//handleState();
		//
		//// update Actor positions
		//updatePositions();
	}
}

void Engine::stop() {
	isGameRunning = false;
	exit(0);
}

// ----------- END CORE FUNCTIONS ------------

int main(int argc, char* argv[]) {
	ConfigUtils configUtils;
	Renderer renderer(configUtils);

	Engine engine(renderer, configUtils);
	engine.start();

	return 0;
}
