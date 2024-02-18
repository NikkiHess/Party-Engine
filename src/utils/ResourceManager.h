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

class ResourceManager {
public:
	ConfigUtils& configUtils;
	SDL_Renderer* sdlRenderer = nullptr;

	ResourceManager(ConfigUtils& configUtils) : configUtils(configUtils) {}

	// load an image texture from its name
	SDL_Texture* loadImageTexture(std::string& imageName);

	// load an text texture from its content
	SDL_Texture* loadTextTexture(std::string& text, SDL_Color fontColor);
};

