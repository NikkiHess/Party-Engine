#pragma once

// my code
#include "GameInfo.h"
#include "GameState.h"
#include "utils/config/ConfigManager.h"
#include "visuals/Renderer.h"

// box2d
#include "box2d/box2d.h"

class Engine {
public:
	static inline bool needsb2World;
	static inline b2World* world;

	Renderer& renderer;
	ConfigManager& configManager;
	ResourceManager& resourceManager;

	bool isGameRunning = false; // is the game running? drives the start loop
	bool gameOver = false;
	bool gameOverMusicPlaying = false;
	GameState state = NORMAL;

	Engine(Renderer& renderer, ConfigManager& configManager, ResourceManager& resourceManager)
		: renderer(renderer), configManager(configManager), resourceManager(resourceManager) {
		GameInfo::state = state;
		GameInfo::scene = configManager.sceneConfig.initialScene;
		GameInfo::newScene = GameInfo::scene;

		GameInfo::configManager = &configManager;
		GameInfo::resourceManager = &resourceManager;
	}

	// run all lifecycle functions
	void runLifecycleFunctions(glm::vec2 mousePos, int clickType);

	// the "alter" part of collect-then-alter
    void runtimeAlterations();

	// start the main game loop
	// LOOP ORDER OF EVENTS:
	// 1.) Input
	// 2.) Move Actors if applicable
	// 3.) Render
	// 4.) Handle render events
	// 5.) Present render (flip)
	void run();

	// request to shut down the main game loop at the end of the frame
	void requestStop();
};
