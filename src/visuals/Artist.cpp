// std library
#include <iostream>

// my code
#include "Artist.h"
#include "Helper.h"
#include "../world/Actor.h"
#include "../GameInfo.h"

void Artist::requestDrawUI(const std::string& imageName, const float x, const float y) {
	SDL_Texture* imageTexture = resourceManager->loadImageTexture(imageName);

	glm::ivec2 pos = {
		static_cast<int>(x),
		static_cast<int>(y)
	};

	resourceManager->createUIImageDrawRequest(imageName, pos, imageTexture);
}

void Artist::requestDrawUIEx(const std::string& imageName, const float x, const float y, const float r,
	const float g, const float b, const float a, float sortingOrder) {
	SDL_Texture* imageTexture = resourceManager->loadImageTexture(imageName);

	glm::ivec2 pos = {
		static_cast<int>(x),
		static_cast<int>(y)
	};

	SDL_Color color = {
		static_cast<int>(r),
		static_cast<int>(g),
		static_cast<int>(b),
		static_cast<int>(a)
	};

	resourceManager->createUIImageDrawRequestEx(imageName, pos, imageTexture, color, sortingOrder);
}

void Artist::draw(const ImageDrawRequest& request) {
	RenderingConfig& renderConfig = configManager->renderingConfig;

	const int pixelsPerMeter = 100;
	glm::vec2 finalRenderPos = glm::vec2(request.pos.x, request.pos.y) - Camera::pos;

	SDL_Texture* texture = request.texture;
	SDL_Rect textureRect;
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
	float yScale = std::abs(request.scale.x);

	textureRect.w *= xScale;
	textureRect.h *= yScale;

	SDL_Point pivotPoint = { 
		static_cast<int>(request.pivot.x * textureRect.w), 
		static_cast<int>(request.pivot.y * textureRect.h) 
	};

	glm::ivec2 cameraDimensions = { 
		renderConfig.renderSize.x, 
		renderConfig.renderSize.y 
	};

	float zoomFactor = request.type == renderConfig.zoomFactor;
	if (request.type == SCENE_SPACE) {
		textureRect.x = static_cast<int>(finalRenderPos.x * pixelsPerMeter + cameraDimensions.x * 0.5f * (1.0f / zoomFactor) - pivotPoint.x);
		textureRect.y = static_cast<int>(finalRenderPos.y * pixelsPerMeter + cameraDimensions.y * 0.5f * (1.0f / zoomFactor) - pivotPoint.y);
	}
	else {
		textureRect.x = static_cast<int>(finalRenderPos.x);
		textureRect.y = static_cast<int>(finalRenderPos.y);
	}

	// apply tint/alpha to texture
	SDL_SetTextureColorMod(texture, request.color.r, request.color.g, request.color.b);
	SDL_SetTextureAlphaMod(texture, request.color.a);

	// draw the texture
	Helper::SDL_RenderCopyEx498(0, "", sdlRenderer, texture, nullptr, &textureRect, request.rotationDegrees, &pivotPoint, static_cast<SDL_RendererFlip>(flipMode));

	// remove tint/alpha from texture
	SDL_SetTextureColorMod(texture, 255, 255, 255);
	SDL_SetTextureAlphaMod(texture, 255);
}

void Artist::requestDrawText(const std::string& text, const float x, const float y, const std::string& fontName,
						   const float fontSize, const float r, const float g, const float b, const float a) {
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
		static_cast<int>(r), 
		static_cast<int>(g),
		static_cast<int>(b),
		static_cast<int>(a)
	};

	glm::ivec2 pos = { 
		static_cast<int>(x), 
		static_cast<int>(y)
	};

	// creates the TextDrawRequest that will be iterated over in the main loop
	resourceManager->createTextDrawRequest(text, font, pos, fontColor);
}

void Artist::drawText(const TextDrawRequest& textDrawRequest) {
	//TTF_Font* font = resourceManager.fonts[textObject.fontName][textObject.fontSize];
	// this is guaranteed to exist at this point
	//SDL_Texture* textTexture = resourceManager.textTextures[font][text];

	int width = 0, height = 0;

	SDL_QueryTexture(textDrawRequest.texture, nullptr, nullptr, &width, &height);

	// create a rect to render the text in
	SDL_Rect textRect = { textDrawRequest.pos.x, textDrawRequest.pos.y, width, height };

	// UI text should always be unscaled
	// need to set this to reset scale
	SDL_RenderSetScale(sdlRenderer, 1, 1);

	// copy the texture to the renderer
	SDL_RenderCopy(sdlRenderer, textDrawRequest.texture, nullptr, &textRect);
}