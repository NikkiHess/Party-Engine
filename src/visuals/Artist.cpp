// std library
#include <iostream>

// my code
#include "Artist.h"
#include "Helper.h"
#include "../world/Actor.h"
#include "../GameInfo.h"

void Artist::setBG(const int r, const int g, const int b) {
	RenderingConfig::clearColor = {r, g, b};
}

void Artist::draw(const ImageDrawRequest& request) {
	RenderingConfig& renderConfig = configManager->renderingConfig;

	const int pixelsPerMeter = 100;
	glm::vec2 finalRenderPos = glm::vec2(request.pos.x, request.pos.y);

	SDL_Texture* texture = request.texture;
	SDL_Rect textureRect = {};
	SDL_QueryTexture(texture, nullptr, nullptr, &textureRect.w, &textureRect.h);

	// apply scale and flip
	int flipMode = SDL_FLIP_NONE;
	if (request.scale.x < 0) {
		flipMode |= SDL_FLIP_HORIZONTAL;
	}
	if (request.scale.y < 0) {
		flipMode |= SDL_FLIP_VERTICAL;
	}

	float xScale = std::abs(request.scale.x);
	float yScale = std::abs(request.scale.y);

	textureRect.w = static_cast<int>(textureRect.w * xScale);
	textureRect.h = static_cast<int>(textureRect.h * yScale);

	SDL_Point pivotPoint = {
		static_cast<int>(request.pivot.x * textureRect.w),
		static_cast<int>(request.pivot.y * textureRect.h)
	};

	glm::ivec2 cameraDimensions = {
		renderConfig.renderSize.x,
		renderConfig.renderSize.y
	};
    
    float zoomFactor = renderConfig.zoomFactor;
	if (request.type == SCENE_SPACE) {
		// only do this with scene space because UI shouldn't move with the camera
		finalRenderPos -= Camera::pos;

		textureRect.x = static_cast<int>(finalRenderPos.x * pixelsPerMeter + cameraDimensions.x * 0.5f * (1.0f / zoomFactor) - pivotPoint.x);
		textureRect.y = static_cast<int>(finalRenderPos.y * pixelsPerMeter + cameraDimensions.y * 0.5f * (1.0f / zoomFactor) - pivotPoint.y);
	}
	else {
		textureRect.x = static_cast<int>(finalRenderPos.x) - pivotPoint.x;
		textureRect.y = static_cast<int>(finalRenderPos.y) - pivotPoint.y;
	}

	// cull
	if ((textureRect.x + textureRect.w < 0 || textureRect.x > cameraDimensions.x * (1.0f / zoomFactor)) ||
		(textureRect.y + textureRect.h < 0 || textureRect.y > cameraDimensions.y * (1.0f / zoomFactor))) {
		return;
	}
	
	// apply tint/alpha to texture
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(texture, request.color.r, request.color.g, request.color.b);
	SDL_SetTextureAlphaMod(texture, request.color.a);

	// draw the texture
	Helper::SDL_RenderCopyEx498(0, "", sdlRenderer, texture, nullptr, &textureRect, request.rotationDegrees, &pivotPoint, static_cast<SDL_RendererFlip>(flipMode));

	// remove tint/alpha from texture
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
	SDL_SetTextureColorMod(texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(texture, 255);
}

void Artist::drawText(const TextDrawRequest& request) {
	int width = 0, height = 0;

	SDL_QueryTexture(request.texture, nullptr, nullptr, &width, &height);

	// create a rect to render the text in
	SDL_Rect textRect = { request.pos.x, request.pos.y, width, height };

	// UI text should always be unscaled
	// need to set this to reset scale
	SDL_RenderSetScale(sdlRenderer, 1, 1);

	// copy the texture to the renderer
	SDL_RenderCopy(sdlRenderer, request.texture, nullptr, &textRect);
}

void Artist::drawPixel(const PixelDrawRequest& request) {
	// set the draw color to the request's color
	SDL_SetRenderDrawColor(sdlRenderer, request.color.r, request.color.g, request.color.b, request.color.a);

	// set the draw mode to blend to ensure alpha works
	SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_BLEND);

	// draw the actual point
	SDL_RenderDrawPoint(sdlRenderer, request.pos.x, request.pos.y);

	// reset the draw mode to none
	SDL_SetRenderDrawBlendMode(sdlRenderer, SDL_BLENDMODE_NONE);
}

int Artist::requestDrawText(const std::string& text, const float x, const float y, const std::string& fontName,
						   const float fontSize, const bool fontCentered, const float r, const float g, const float b, const float a) {
	const int fontSizeInt = static_cast<int>(fontSize);

	// if the font doesn't exist, create it
	const auto& fontMapIt = resourceManager->fonts.find(fontName);
	if (fontMapIt == resourceManager->fonts.end()) {
		// initialize the unordered_map
		resourceManager->fonts[fontName] = {};
	}
	// if the font size doesn't exist, create it
	const auto& fontIt = resourceManager->fonts[fontName].find(fontSizeInt);
	if (fontIt == resourceManager->fonts[fontName].end()) {
		// open the font and add it to the map
		const std::string fontPath = "resources/fonts/" + fontName + ".ttf";
		resourceManager->fonts[fontName][fontSizeInt] = TTF_OpenFont(fontPath.c_str(), fontSizeInt);
	}

	TTF_Font* font = resourceManager->fonts[fontName][fontSizeInt];
	SDL_Color fontColor = { 
		static_cast<Uint8>(r), 
		static_cast<Uint8>(g),
		static_cast<Uint8>(b),
		static_cast<Uint8>(a)
	};

	glm::ivec2 pos = { 
		static_cast<int>(x), 
		static_cast<int>(y)
	};

	// creates the TextDrawRequest that will be iterated over in the main loop
	return resourceManager->createTextDrawRequest(text, font, fontCentered, pos, fontColor);
}

int Artist::requestDrawUI(const std::string& imageName, const float x, const float y) {
	SDL_Texture* imageTexture = resourceManager->loadImageTexture(imageName);

	glm::ivec2 pos = {
		static_cast<int>(x),
		static_cast<int>(y)
	};

	return resourceManager->createUIImageDrawRequest(imageTexture, imageName, pos);
}

int Artist::requestDrawUIEx(const std::string& imageName, const float x, const float y, const float rotationDegrees,
							const float scaleX, const float scaleY, const float pivotX, const float pivotY, const float r,
							const float g, const float b, const float a, float sortingOrder) {
	SDL_Texture* imageTexture = resourceManager->loadImageTexture(imageName);

	glm::ivec2 pos = {
		static_cast<int>(x),
		static_cast<int>(y)
	};

	glm::vec2 scale = { scaleX, scaleY };
	glm::vec2 pivot = { pivotX, pivotY };

	SDL_Color color = {
		static_cast<Uint8>(r),
		static_cast<Uint8>(g),
		static_cast<Uint8>(b),
		static_cast<Uint8>(a)
	};

	return resourceManager->createUIImageDrawRequestEx(imageTexture, imageName, pos, rotationDegrees, scale, pivot, color, static_cast<int>(sortingOrder));
}

int Artist::requestDrawImage(const std::string& imageName, const float x, const float y) {
	SDL_Texture* imageTexture = resourceManager->loadImageTexture(imageName);

	glm::vec2 pos = { x, y };

	return resourceManager->createImageDrawRequest(imageTexture, imageName, pos);
}

int Artist::requestDrawImageEx(const std::string& imageName, const float x, const float y, const float rotationDegrees,
								const float scaleX, const float scaleY, const float pivotX, const float pivotY,
								const float r, const float g, const float b, const float a, const float sortingOrder) {
	SDL_Texture* imageTexture = resourceManager->loadImageTexture(imageName);

	glm::vec2 pos = { x, y };

	glm::vec2 scale = { scaleX, scaleY };

	glm::vec2 pivot = { pivotX, pivotY };

	SDL_Color color = {
		static_cast<Uint8>(r),
		static_cast<Uint8>(g),
		static_cast<Uint8>(b),
		static_cast<Uint8>(a)
	};

	return resourceManager->createImageDrawRequestEx(imageTexture, imageName, pos, rotationDegrees, scale,
											  pivot, color, static_cast<int>(sortingOrder));
}

void Artist::requestDrawPixel(const float x, const float y, const float r, const float g, const float b, const float a) {
	glm::ivec2 pos = { x, y };

	SDL_Color color = {
		static_cast<Uint8>(r),
		static_cast<Uint8>(g),
		static_cast<Uint8>(b),
		static_cast<Uint8>(a)
	};
	
	resourceManager->createPixelDrawRequest(pos, color);
}

int Artist::getImageWidth(const std::string imageName) {
	SDL_Texture* texture = resourceManager->loadImageTexture(imageName);

	int width = 0;
	SDL_QueryTexture(texture, nullptr, nullptr, &width, nullptr);
	return width;
}

int Artist::getImageHeight(const std::string imageName) {
	SDL_Texture* texture = resourceManager->loadImageTexture(imageName);

	int height = 0;
	SDL_QueryTexture(texture, nullptr, nullptr, nullptr, &height);
	return height;
}
