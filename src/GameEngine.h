#pragma once

// std stuff
#include <string>
#include <map>

// my code
#include "Renderer.h"
#include "utils/ConfigHelper.h"
#include "gamedata/Actor.h"
#include "gamedata/GameInfo.h"

// dependencies
#include "glm/glm.hpp"

class Engine {
public:
	Renderer& renderer;
	ConfigHelper& configHelper;
	bool isGameRunning = false; // is the game running? drives the start loop
	GameState state = NORMAL;

	// player stuff
	Actor* player = nullptr; // the player

	int playerHealth = 3; // the player's current health
	int playerScore = 0; // the player's current score

	// load the game info after everything else has been loaded
	GameInfo gameInfo{ player, playerHealth, playerScore,
						state, configHelper.gameStartMessage,
						configHelper.gameOverBadMessage, configHelper.gameOverGoodMessage, configHelper.initialScene };

	Engine(Renderer& renderer, ConfigHelper& configHelper) : renderer(renderer), configHelper(configHelper) {
		std::vector<Actor>& actors = configHelper.initialScene.actors;
		// this finds the player in the actors map
		auto playerIt = std::find_if(actors.begin(), actors.end(), [](Actor actor) { return actor.actorName == "player"; });

		if (playerIt == actors.end()) {
			std::cout << "error: player not defined";
			exit(0);
		}
		// sets the player from the actors vector
		player = &*playerIt;
		gameInfo.player = player;
	}

	// execute the main game loop
	void start();

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
