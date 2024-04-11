#pragma once

// std library
#include <string>
#include <filesystem>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <vector>

// my code
#include "../components/CppComponent.h"

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
	SDL_Texture* texture = nullptr;
	std::string name = "";
	glm::vec2 pos = { 0, 0 };
	int rotationDegrees = 0;
	glm::vec2 scale = { 1, 1 };
	glm::vec2 pivot = { 0.5f, 0.5f };
	SDL_Color color = { 255, 255, 255, 255 };

	int sortingOrder = 0;
	int callOrder = 0;

	ImageType type = SCENE_SPACE;

	ImageDrawRequest() {}

	ImageDrawRequest(SDL_Texture* texture, const std::string& name, glm::vec2 pos, int rotationDegrees, glm::vec2 scale, glm::vec2 pivot,
		SDL_Color& color, int sortingOrder, int callOrder, ImageType type)
		: name(name), pos(pos), texture(texture), rotationDegrees(rotationDegrees), scale(scale), pivot(pivot), color(color), sortingOrder(sortingOrder), callOrder(callOrder), type(type) {}
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

class PixelDrawRequest {
public:
	glm::ivec2 pos = { 0, 0 };
	SDL_Color color = { 255, 255, 255, 255 };

	PixelDrawRequest() {};

	PixelDrawRequest(glm::ivec2& pos, SDL_Color& color) : pos(pos), color(color) {};
};