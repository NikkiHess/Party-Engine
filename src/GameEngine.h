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
		: renderer(renderer), configManager(configManager), audioPlayer(audioPlayer), input(input), camera(camera) {
		
		std::vector<Actor>& actors = configManager.sceneConfig.initialScene.actors;
		// this finds the player in the actors map
		auto playerIt = std::find_if(actors.begin(), actors.end(), [](Actor actor) { return actor.name == "player"; });

		if (playerIt != actors.end()) {
			player = &*playerIt;
			// set the player's speed from the config
			player->speed = configManager.gameConfig.playerSpeed;
			gameInfo.player = player;

			// preload the player view to calculate the position
			player->view.image = resourceManager.loadImageTexture(player->view.imageName);

			camera.jump(player);
		}
	}

	// start the main game loop
	void start();

	// shut down the main game loop
	void queueStop();
};
