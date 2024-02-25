#pragma once

// std stuff
#include <vector>
#include <string>

// my code
#include "../GameInfo.h"
#include "../utils/config/ConfigManager.h"
#include "../utils/config/GameConfig.h"
#include "../utils/ResourceManager.h"
#include "Artist.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL.h"
#include "Helper.h"

class Renderer {
public:
	SDL_Renderer* sdlRenderer = nullptr;
	ConfigManager& configManager;
	ResourceManager& resourceManager;

	Artist artist; // responsible for drawing stuff

	Renderer(ConfigManager& configManager, ResourceManager& resourceManager) 
		: configManager(configManager), resourceManager(resourceManager), artist(configManager, resourceManager) {
		GameConfig& gameConfig = configManager.gameConfig;
		RenderingConfig& renderConfig = configManager.renderingConfig;

		// a window with proprties as defined by configManager
		SDL_Window* window = SDL_CreateWindow(
			gameConfig.gameTitle.c_str(),	// window title
			SDL_WINDOWPOS_CENTERED,			// initial x
			SDL_WINDOWPOS_CENTERED,			// iniital y
			renderConfig.renderSize.x,		// width, in pixels
			renderConfig.renderSize.y,		// height, in pixels
			SDL_WINDOW_SHOWN				// flags
		);

		// Create our Renderer using our window, -1 (go find a display), and VSYNC/GPU rendering enabled
		sdlRenderer = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
		artist.sdlRenderer = sdlRenderer;
		artist.resourceManager.sdlRenderer = sdlRenderer;
	}

	// render the current view, returns the current GameState
	void render(GameInfo& gameInfo);
};