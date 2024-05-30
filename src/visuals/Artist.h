#pragma once

// std library
#include <string>

// my code
#include "../utils/config/ConfigManager.h"
#include "../utils/ResourceManager.h"

// handles drawing of visuals (not displaying, that's Renderer's job)
class Artist
{
public:
	static inline ConfigManager* configManager;
	static inline SDL_Renderer* sdlRenderer;
	static inline ResourceManager* resourceManager;

	static void draw(const ImageDrawRequest& request);
	static void drawText(const TextDrawRequest& request);
	static void drawPixel(const PixelDrawRequest& request);

	// set the bg (clear) color
	static void setBG(const int r, const int g, const int b);

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
    // justification - left, center, right
    // bold - whether or not to bold the text
    // underline - whether or not to underline the text
    // italic - whether or not to italicize the text
    // strikethrough - whether or not to strikethrough the text
	static int requestDrawText(const std::string& text, const float x, const float y, const std::string& fontName,
		const float fontSize, const float r, const float g, const float b, const float a, const std::string horizJust, const std::string vertJust, const bool bold, const bool underline, const bool italic, const bool strikethrough);

	// request to draw an image on the screen
	static int requestDrawUI(const std::string& imageName, const float x, const float y);

	// request to draw an image on the screen (extended, more params)
	static int requestDrawUIEx(const std::string& imageName, const float x, const float y, const float rotationDegrees,
							   const float scaleX, const float scaleY, const float pivotX, const float pivotY, const float r,
							   const float g, const float b, const float a, float sortingOrder);

	static int requestDrawImage(const std::string& imageName, const float x, const float y);

	static int requestDrawImageEx(const std::string& imageName, const float x, const float y, const float rotationDegrees,
								   const float scaleX, const float scaleY, const float pivotX, const float pivotY,
								   const float r, const float g, const float b, const float a, const float sortingOrder);

	// rqeuest to draw a single pixel on the screen
	static void requestDrawPixel(const float x, const float y, const float r, const float g, const float b, const float a);

	static int getImageWidth(const std::string imageName);
	static int getImageHeight(const std::string imageName);
};

