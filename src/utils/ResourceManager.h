#pragma once

// std library
#include <string>
#include <filesystem>
#include <unordered_map>

// my code
#include "../gamedata/Actor.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"

class ResourceManager {
public:
	SDL_Renderer* sdlRenderer = nullptr;

	// data cache
	std::unordered_map<std::string, SDL_Texture*> imageTextures, textTextures;
	std::unordered_map<std::string, Mix_Chunk*> sounds;
	TTF_Font* font = nullptr;
	std::unordered_map<std::string, bool> fileExistsCache;

	ResourceManager() {}

	// load an image texture from its name
	SDL_Texture* loadImageTexture(std::string& imageName);

	// load an text texture from its content
	SDL_Texture* loadTextTexture(std::string& text, SDL_Color fontColor);

	// returns whether a file exists (from cache or otherwise)
	bool fileExists(const std::string& path);
};

