#pragma once

// std library
#include <string>
#include <map>
#include <algorithm>

// my code
#include "visuals/Renderer.h"
#include "utils/ConfigUtils.h"
#include "gamedata/Actor.h"
#include "gamedata/GameInfo.h"

// dependencies
#include "glm/glm.hpp"

class Engine {
public:
	Renderer& renderer;
	ConfigUtils& configUtils;
	bool isGameRunning = false; // is the game running? drives the start loop
	GameState state = NORMAL;

	// player stuff
	Actor* player = nullptr; // the player

	// load the game info after everything else has been loaded
	GameInfo gameInfo{ 
		player,
		state,
		configUtils.initialScene 
	};

	Engine(Renderer& renderer, ConfigUtils& configUtils) : renderer(renderer), configUtils(configUtils) {
		std::vector<Actor>& actors = configUtils.initialScene.actors;
		// this finds the player in the actors map
		auto playerIt = std::find_if(actors.begin(), actors.end(), [](Actor actor) { return actor.name == "player"; });

		if (playerIt != actors.end()) {
			player = &*playerIt;
			gameInfo.player = player;
		}
	}

	// initialize the game window, renderer, and start the main game loop
	void start();

	// execute the main game loop
	void doGameLoop();

	// shut down the main game loop
	void stop();

	// update all Actor positions according to their velocity
	void updatePositions();

	// returns whether an actor would collide given its velocity
	bool wouldCollide(Actor& actor);

	// handles the current state
	// NORMAL = do nothing
	// WIN or LOSE = exit 
	void handleState();
private:
	// update the position of a specific actor (used in update_positions)
	void updateActorPosition(Actor& actor);
};
