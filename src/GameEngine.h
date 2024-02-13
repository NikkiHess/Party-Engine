#pragma once

// std library
#include <algorithm>
#include <map>
#include <string>

// my code
#include "audio/AudioPlayer.h"
#include "gamedata/Actor.h"
#include "gamedata/GameInfo.h"
#include "utils/ConfigUtils.h"
#include "visuals/Renderer.h"

// dependencies
#include "glm/glm.hpp"

class Engine {
public:
	Renderer& renderer;
	ConfigUtils& configUtils;
	AudioPlayer& audioPlayer;
	bool isGameRunning = false; // is the game running? drives the start loop
	GameState state = NORMAL;
	bool gameOver = false;
	bool gameOverMusicPlaying = false;

	// player stuff
	Actor* player = nullptr; // the player

	// load the game info after everything else has been loaded
	GameInfo gameInfo{ 
		player,
		state,
		configUtils.initialScene,
	};

	Engine(Renderer& renderer, ConfigUtils& configUtils, AudioPlayer& audioPlayer) : renderer(renderer), configUtils(configUtils), audioPlayer(audioPlayer) {
		std::vector<Actor>& actors = configUtils.initialScene.actors;
		// this finds the player in the actors map
		auto playerIt = std::find_if(actors.begin(), actors.end(), [](Actor actor) { return actor.name == "player"; });

		if (playerIt != actors.end()) {
			player = &*playerIt;
			gameInfo.player = player;
		}
	}

	// preload and cache all resources (images, sounds, fonts)
	void preloadResources();

	// initialize the game window, renderer, and start the main game loop
	void start();

	// execute the main game loop
	void doGameLoop();

	// shut down the main game loop
	void stop();

	// update all NPC Actor positions according to their velocity
	void updateNPCPositions();

	// returns whether an actor would collide given its velocity
	bool wouldCollide(Actor* actor);
private:
	// update the position of a specific actor (used in update_positions)
	void updateActorPosition(Actor* actor);
};
