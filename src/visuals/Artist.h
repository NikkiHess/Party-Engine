#pragma once

// std library
#include <string>

// my code
#include "../gamedata/Actor.h"
#include "../utils/ConfigUtils.h"
#include "../gamedata/GameInfo.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"

// handles drawing of visuals (not displaying, that's Renderer's job)
class Artist
{
public:
	SDL_Renderer* sdlRenderer; // the SDL renderer object passed in from Renderer
	ConfigUtils& configUtils; // the ConfigUtils passed in from Renderer

	Artist(ConfigUtils& configUtils) : configUtils(configUtils) {};

	// load an image texture from its name
	SDL_Texture* loadImageTexture(std::string& imageName);

	// draw an unchanging image on the screen
	void drawStaticImage(std::string& imageName, glm::ivec2 pos, glm::ivec2 size);

	// draw an unchanging image on the screen
	void drawActor(GameInfo& gameInfo, Actor& actor);

	// draw text on the screen
	void drawText(std::string& text, int fontSize, SDL_Color fontColor, glm::ivec2 pos);
};

