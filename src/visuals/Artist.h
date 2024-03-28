#pragma once

// std library
#include <string>

// my code
#include "Camera.h"
#include "../world/Actor.h"
#include "../utils/config/ConfigManager.h"
#include "../GameInfo.h"
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
	static inline ConfigManager* configManager;
	static inline SDL_Renderer* sdlRenderer;
	static inline ResourceManager* resourceManager;

	// request to draw an image on the screen
	static void requestDrawUI(const std::string& imageName, const float x, const float y);

	// request to draw an image on the screen (extended, more params)
	static void requestDrawUIEx(const std::string& imageName, const float x, const float y, const float r, 
								const float g, const float b, const float a, float sortingOrder);

	static void draw(const ImageDrawRequest& uiImageDrawRequest);

	// request text to be drawn on the screen
	// text - the text to be drawn
	// x - the (screen) x coordinate to draw at
	// y - the (screen) y coordinate to draw at
	// fontName - the name of the font to be used (looked up then applied)
	// fontSize - the size of the font to be used (looked up then applied)
	// r - the red value of the text color
	// g - the blue value of the text color
	// b - the green value of the text color
	// a - the alpha value of the text color (transparency)
	static void requestDrawText(const std::string& text, const float x, const float y, const std::string& fontName,
		const float fontSize, const float r, const float g, const float b, const float a);

	// actually draw the text
	static void drawText(const TextDrawRequest& textDrawRequest);
};

