#pragma once

// std library
#include <string>

// my code
#include "../gamedata/Actor.h"
#include "../utils/config/ConfigManager.h"
#include "../gamedata/GameInfo.h"
#include "../utils/ResourceManager.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"

// handles drawing of visuals (not displaying, that's Renderer's job)
class Artist
{
public:
	ConfigManager& configManager; // the ConfigManager passed in from Renderer
	SDL_Renderer* sdlRenderer = nullptr; // the SDL renderer object passed in from Renderer
	ResourceManager& resourceManager;

	static const int PIXELS_PER_UNIT = 100;

	Artist(ConfigManager& configManager, ResourceManager& resourceManager) : configManager(configManager), resourceManager(resourceManager) {}

	// draw an unchanging image on the screen
	void drawUIImage(std::string& imageName, glm::ivec2 pos, glm::ivec2 size);

	// draw an unchanging image on the screen
	void drawActor(GameInfo& gameInfo, Actor& actor);

	// draw text on the screen
	void drawUIText(std::string& text, SDL_Color fontColor, glm::ivec2 pos);
};

