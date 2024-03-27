#pragma once

// std library
#include <string>
#include <filesystem>
#include <unordered_map>
#include <set>

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"

class TextDrawRequest {
public:
	std::string text;
	glm::ivec2 pos;
	TTF_Font* font;
	SDL_Color color;
	SDL_Texture* texture;

	TextDrawRequest() : text(""), pos(0, 0), font(nullptr), color(), texture(nullptr) {}

	TextDrawRequest(const std::string& text, glm::ivec2& pos, TTF_Font* font, SDL_Color& color, SDL_Texture* texture)
		: text(text), pos(pos), font(font), color(color), texture(texture) {}

	TextDrawRequest& operator=(const TextDrawRequest& other) {
		text = other.text;
		pos = other.pos;
		font = other.font;
		color = other.color;
		texture = other.texture;

		return *this;
	}
};

enum ImageType {
	SCENE_SPACE, SCREEN_SPACE
};

class ImageDrawRequest {
public:
	std::string name = "";
	glm::vec2 pos = { 0, 0 };
	SDL_Texture* texture = nullptr;
	int rotationDegrees = 0;
	glm::vec2 scale = { 1, 1 };
	glm::vec2 pivot = { 0.5f, 0.5f };
	SDL_Color color = { 255, 255, 255, 255 };

	int sortingOrder = 0;
	int callOrder = 0;

	ImageType type = SCENE_SPACE;

	ImageDrawRequest() {}

	// for UI images
	ImageDrawRequest(const std::string& name, glm::ivec2& pos, SDL_Texture* texture, int callOrder) : name(name), pos(pos), texture(texture), callOrder(callOrder) {
		type = SCREEN_SPACE;
	}

	ImageDrawRequest(const std::string& name, glm::ivec2& pos, SDL_Texture* texture, int rotationDegrees, glm::vec2& scale, glm::vec2& pivot,
					 SDL_Color& color, int sortingOrder, int callOrder)
					: name(name), pos(pos), texture(texture), rotationDegrees(rotationDegrees), scale(scale), pivot(pivot), color(color), sortingOrder(sortingOrder), callOrder(callOrder) {}
};

class ImageDrawRequestComparator {
public:
	bool operator()(const ImageDrawRequest& request1, const ImageDrawRequest& request2) const {
		if (request1.sortingOrder == request2.sortingOrder) {
			return request1.callOrder < request2.callOrder;
		}
		return request1.sortingOrder < request2.sortingOrder;
	}
};

class ResourceManager {
public:
	SDL_Renderer* sdlRenderer = nullptr;

	// data cache
	std::unordered_map<std::string, SDL_Texture*> imageTextures;

	std::vector<TextDrawRequest> textDrawRequests;
	std::vector<ImageDrawRequest> uiImageDrawRequests;

	// stored as {font, {text, texture}}
	//std::unordered_map<TTF_Font*, std::unordered_map<SDL_Color, std::unordered_map<std::string, SDL_Texture*>>> textTextures;
	std::unordered_map<std::string, Mix_Chunk*> sounds;

	// stored as {fontName, {fontSize, font}}
	std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;

	std::unordered_map<std::string, bool> fileExistsCache;

	ResourceManager() {}

	// load an image texture from its name
	SDL_Texture* loadImageTexture(const std::string& imageName);

	// create a UI image draw request, to be drawn at the end of the frame
	ImageDrawRequest createUIImageDrawRequest(const std::string& imageName, glm::ivec2& pos, SDL_Texture* imageTexture);

	// create a text draw request, to be drawn at the end of the frame
	TextDrawRequest createTextDrawRequest(const std::string& text, TTF_Font* font, glm::ivec2& pos, SDL_Color& fontColor);

	// returns whether a file exists (from cache or otherwise)
	bool fileExists(const std::string& path);
};

