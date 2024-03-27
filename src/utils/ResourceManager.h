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

	TextDrawRequest(const std::string& text, glm::ivec2& pos, TTF_Font* font, SDL_Color color, SDL_Texture* texture)
		: text(text), pos(pos), font(font), color(color), texture(texture) {
	}

	TextDrawRequest& operator=(const TextDrawRequest& other) {
		text = other.text;
		pos = other.pos;
		font = other.font;
		color = other.color;
		texture = other.texture;

		return *this;
	}
};

class TextDrawRequestComparator {
public:
	bool operator()(const TextDrawRequest& text1, const TextDrawRequest& text2) const {
		return text1.text < text2.text;
	}
};

class ResourceManager {
public:
	SDL_Renderer* sdlRenderer = nullptr;

	// data cache
	std::unordered_map<std::string, SDL_Texture*> imageTextures;

	std::set<TextDrawRequest, TextDrawRequestComparator> textDrawRequests;

	// stored as {font, {text, texture}}
	//std::unordered_map<TTF_Font*, std::unordered_map<SDL_Color, std::unordered_map<std::string, SDL_Texture*>>> textTextures;
	std::unordered_map<std::string, Mix_Chunk*> sounds;

	// stored as {fontName, {fontSize, font}}
	std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;

	std::unordered_map<std::string, bool> fileExistsCache;

	ResourceManager() {}

	// load an image texture from its name
	SDL_Texture* loadImageTexture(std::string& imageName);

	// load an text texture from its content
	TextDrawRequest& createTextDrawRequest(const std::string& text, TTF_Font* font, glm::ivec2 pos, SDL_Color& fontColor);

	// returns whether a file exists (from cache or otherwise)
	bool fileExists(const std::string& path);
};

