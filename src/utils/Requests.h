#pragma once

// std library
#include <string>

// dependencies
#include "glm/glm.hpp" 
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"

class TextDrawRequest {
public:
	static inline int numRequests = 0;

	std::string text;
	glm::ivec2 pos;
	TTF_Font* font;
	SDL_Color color;
	SDL_Texture* texture;

	int id = 0;

	TextDrawRequest() : text(""), pos(0, 0), font(nullptr), color(), texture(nullptr) {}

	TextDrawRequest(const std::string& text, glm::ivec2& pos, TTF_Font* font, SDL_Color& color, SDL_Texture* texture)
		: text(text), pos(pos), font(font), color(color), texture(texture) {
		id = numRequests++;
	}
};

class TextDrawRequestComparator {
public:
	bool operator()(const TextDrawRequest& request1, const TextDrawRequest& request2) const {
		return request1.id < request2.id;
	}
};

enum ImageType {
	SCENE_SPACE, SCREEN_SPACE
};

class ImageDrawRequest {
public:
	static inline int numRequests = 0;

	SDL_Texture* texture = nullptr;
	std::string name = "";
	glm::vec2 pos = { 0, 0 };
	float rotationDegrees = 0;
	glm::vec2 scale = { 1, 1 };
	glm::vec2 pivot = { 0.5f, 0.5f };
	SDL_Color color = { 255, 255, 255, 255 };

	int sortingOrder = 0;
	size_t callOrder = 0;
	int id = 0;

	ImageType type = SCENE_SPACE;

	ImageDrawRequest() {}

	ImageDrawRequest(SDL_Texture* texture, const std::string& name, glm::vec2 pos, float rotationDegrees, glm::vec2 scale, glm::vec2 pivot,
		SDL_Color& color, int sortingOrder, size_t callOrder, ImageType type)
		: texture(texture), name(name), pos(pos), rotationDegrees(rotationDegrees), scale(scale),
		pivot(pivot), color(color), sortingOrder(sortingOrder), callOrder(callOrder), type(type) {
		id = numRequests++;
	}
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
	static inline int numRequests = 0;

	glm::ivec2 pos = { 0, 0 };
	SDL_Color color = { 255, 255, 255, 255 };

	int id = 0;

	PixelDrawRequest() {};

	PixelDrawRequest(glm::ivec2& pos, SDL_Color& color) : pos(pos), color(color) {
		id = numRequests++;
	};
};


class PixelDrawRequestComparator {
public:
	bool operator()(const PixelDrawRequest& request1, const PixelDrawRequest& request2) const {
		return request1.id < request2.id;
	}
};
