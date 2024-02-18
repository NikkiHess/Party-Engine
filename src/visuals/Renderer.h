#pragma once

// std stuff
#include <vector>

// my code
#include "../gamedata/GameInfo.h"
#include "../utils/ConfigUtils.h"
#include "Artist.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL.h"
#include "Helper.h"

class Renderer {
public:
	SDL_Renderer* sdlRenderer = nullptr; // the sdl renderer we're using
	ConfigUtils& configUtils; // the ConfigUtils the game uses
	glm::ivec2 renderSize; // the size of the rendered view

	Artist artist; // responsible for drawing stuff

	Renderer(ConfigUtils& configUtils) : configUtils(configUtils), renderSize(configUtils.renderSize), artist(configUtils) {
		// a window with proprties as defined by configUtils
		SDL_Window* window = SDL_CreateWindow(
			configUtils.gameTitle.c_str(),	// window title
			SDL_WINDOWPOS_CENTERED,			// initial x
			SDL_WINDOWPOS_CENTERED,			// iniital y
			configUtils.renderSize.x,		// width, in pixels
			configUtils.renderSize.y,		// height, in pixels
			SDL_WINDOW_SHOWN				// flags
		);

		// Create our Renderer using our window, -1 (go find a display), and VSYNC/GPU rendering enabled
		sdlRenderer = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
		artist.sdlRenderer = sdlRenderer;
		artist.resourceManager.sdlRenderer = sdlRenderer;
	}

	// render the intro images one by one, proceed when enter has been pressed
	void renderIntro(int& index);

	// render the current view, returns the current GameState
	void render(GameInfo& gameInfo);

	// render the HUD
	void renderHUD(GameInfo& gameInfo);

	// render any dialogue from nearby Actors, returns the current GameState
	// as a result of any dialogue commands executed via executeCommands
	void renderDialogue(GameInfo& gameInfo);
private:
	// execute the commands contained in the dialogue string with the given
	// Actor as a trigger
	GameState executeCommands(Actor& trigger, const std::string& dialogue, GameInfo& gameInfo);
};