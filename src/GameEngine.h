#pragma once

// std library
#include <algorithm>
#include <map>
#include <string>

// my code
#include "audio/AudioPlayer.h"
#include "gamedata/Actor.h"
#include "gamedata/GameInfo.h"
#include "utils/config/ConfigManager.h"
#include "visuals/Renderer.h"
#include "gamedata/Input.h"

// dependencies
#include "glm/glm.hpp"

class Engine {
public:
	Renderer& renderer;
	ConfigManager& configManager;
	AudioPlayer& audioPlayer;
	Input& input;
	Camera& camera;
	ResourceManager& resourceManager;

	bool isGameRunning = false; // is the game running? drives the start loop
	GameState state = NORMAL;
	bool gameOver = false;
	bool gameOverMusicPlaying = false;
	Actor* player = nullptr; // the player

	// load the game info after everything else has been loaded
	GameInfo gameInfo { 
		player,
		state,
		configManager.sceneConfig.initialScene,
		camera
	};

	Engine(Renderer& renderer, ConfigManager& configManager, AudioPlayer& audioPlayer, Input& input, Camera& camera, ResourceManager& resourceManager) 
		: renderer(renderer), configManager(configManager), audioPlayer(audioPlayer), input(input), camera(camera), resourceManager(resourceManager) {
		
		std::vector<Actor>& actors = configManager.sceneConfig.initialScene.actors;
		// this finds the player in the actors map
		auto playerIt = std::find_if(actors.begin(), actors.end(), [](Actor actor) { return actor.name == "player"; });

		if (playerIt != actors.end()) {
			player = &*playerIt;
			// set the player's speed from the config
			player->speed = configManager.gameConfig.playerSpeed;
			gameInfo.player = player;

			// preload the player view to calculate the position
			player->view.imageFront.image = resourceManager.loadImageTexture(player->view.imageFront.name);
			if (player->view.imageBack.name != "") {
				player->view.imageBack.image = resourceManager.loadImageTexture(player->view.imageBack.name);
			}

			camera.jump(player);
		}
	}

	// start the main game loop
	// LOOP ORDER OF EVENTS:
	// 1.) Input
	// 2.) Move Actors if applicable
	// 3.) Render
	// 4.) Handle render events
	// 5.) Present render (flip)
	void start();

	// shut down the main game loop
	void queueStop();
};
