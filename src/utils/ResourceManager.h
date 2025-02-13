#pragma once

// std library
#include <filesystem> 
#include <set>
#include <string>
#include <unordered_map>

// my code
#include "Requests.h"

// dependencies
#include "glm/glm.hpp" 
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"

class ResourceManager {
public:
	SDL_Renderer* sdlRenderer = nullptr;

	// data cache
	std::unordered_map<std::string, SDL_Texture*> imageTextures;

	std::set<TextDrawRequest, TextDrawRequestComparator> textDrawRequests;
	std::set<ImageDrawRequest, ImageDrawRequestComparator> uiImageDrawRequests;
	std::set<ImageDrawRequest, ImageDrawRequestComparator> imageDrawRequests;
	std::set<PixelDrawRequest, PixelDrawRequestComparator> pixelDrawRequests;

	// stored as {font, {text, texture}}
	//std::unordered_map<TTF_Font*, std::unordered_map<SDL_Color, std::unordered_map<std::string, SDL_Texture*>>> textTextures;
	std::unordered_map<std::string, Mix_Chunk*> sounds;

	// stored as {fontName, {fontSize, font}}
	std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;

	std::unordered_map<std::string, bool> fileExistsCache;

	ResourceManager() {}

	~ResourceManager() {
		for (auto& [name, texture] : imageTextures) {
			SDL_DestroyTexture(texture);
		}
		imageTextures.clear();
	}

	// load an image texture from its name
	SDL_Texture* loadImageTexture(const std::string& imageName);

	// create a text draw request, to be drawn at the end of the frame
	int createTextDrawRequest(const std::string& text, TTF_Font* font, glm::ivec2& pos, SDL_Color& fontColor, const std::string horizJust, const std::string vertJust, const bool bold, const bool underline, const bool italic, const bool strikethrough);

	// create a UI image draw request, to be drawn at the end of the frame
	int createUIImageDrawRequest(SDL_Texture* imageTexture, const std::string& imageName, glm::ivec2& pos);

	// create a UI image draw request (with more data), to be drawn at the end of the frame
	int createUIImageDrawRequestEx(SDL_Texture* imageTexture, const std::string& imageName, glm::ivec2& pos, float rotationDegrees, 
								   glm::vec2& scale, glm::vec2& pivot, SDL_Color color, int sortingOrder);

	// create an image draw request, to be drawn at the end of the frame
	int createImageDrawRequest(SDL_Texture* imageTexture, const std::string& imageName, glm::vec2& pos);

	// create an image draw request (with more data), to be drawn at the end of the frame
	int createImageDrawRequestEx(SDL_Texture* imageTexture, const std::string& imageName, glm::vec2& pos, float rotationDegrees,
								  glm::vec2& scale, glm::vec2& pivot, SDL_Color color, int sortingOrder);

	// create a pixel draw request, to be a drawn at the end of the frame
	void createPixelDrawRequest(glm::ivec2& pos, SDL_Color color);

	// returns whether a file exists (from cache or otherwise)
	bool fileExists(const std::string& path);
};

