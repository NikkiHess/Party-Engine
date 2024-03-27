#pragma once

// std library
#include <algorithm>
#include <map>
#include <string>

// my code
#include "audio/AudioPlayer.h"
#include "world/Actor.h"
#include "GameInfo.h"
#include "GameState.h"
#include "utils/config/ConfigManager.h"
#include "visuals/Renderer.h"
#include "input/Input.h"

// dependencies
#include "glm/glm.hpp"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class Engine {
public:
	Renderer& renderer;
	ConfigManager& configManager;
	AudioPlayer& audioPlayer;
	Camera& camera;
	ResourceManager& resourceManager;

	bool isGameRunning = false; // is the game running? drives the start loop
	GameState state = NORMAL;
	bool gameOver = false;
	bool gameOverMusicPlaying = false;

	// load the game info after everything else has been loaded
	GameInfo gameInfo{
		state,
		configManager.sceneConfig.initialScene,
		camera
	};

	Engine(Renderer& renderer, ConfigManager& configManager, AudioPlayer& audioPlayer, Camera& camera, ResourceManager& resourceManager, lua_State* luaState)
		: renderer(renderer), configManager(configManager), audioPlayer(audioPlayer), camera(camera), resourceManager(resourceManager) {}


	// run OnStart, OnUpdate, OnLateUpdate
	void runLifecycleFunctions();

	// the "alter" part of collect-then-alter
    void runtimeAlterations();

	// start the main game loop
	// LOOP ORDER OF EVENTS:
	// 1.) Input
	// 2.) Move Actors if applicable
	// 3.) Render
	// 4.) Handle render events
	// 5.) Present render (flip)
	void start();

	// request to shut down the main game loop at the end of the frame
	void requestStop();
};
