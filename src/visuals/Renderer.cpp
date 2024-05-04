// std library
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>

// include my code
#include "../world/Actor.h"
#include "../GameInfo.h"
#include "../GameEngine.h"
#include "../utils/StringUtils.h"
#include "Renderer.h"

// dependencies
#include "AudioHelper.h"
#include "glm/glm.hpp"
#include "Helper.h"
#include "rapidjson/document.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_ttf.h"

void Renderer::render() {
	RenderingConfig& renderConfig = configManager.renderingConfig;

	// Clear the frame buffer at the beginning of a frame
	SDL_SetRenderDrawColor(
		sdlRenderer, 
		renderConfig.clearColor.r,
		renderConfig.clearColor.g,
		renderConfig.clearColor.b,
		1
	);
	SDL_RenderClear(sdlRenderer);

	// RENDER ORDER:
	// 1. screen-space images (Image.Draw/Image.DrawEx)
	// set the render scale according to the configured zoom factor
	SDL_RenderSetScale(sdlRenderer, renderConfig.zoomFactor, renderConfig.zoomFactor);
	//std::stable_sort(resourceManager.imageDrawRequests.begin(), resourceManager.imageDrawRequests.end(), imgComp);
	for (const ImageDrawRequest& image : resourceManager.imageDrawRequests) {
		Artist::draw(image);
	}
	resourceManager.imageDrawRequests.clear();

	// 2. UI images (Image.DrawUI/Image.DrawUIEx)
	// set the render scale to 1
	SDL_RenderSetScale(sdlRenderer, 1, 1);
	//std::stable_sort(resourceManager.uiImageDrawRequests.begin(), resourceManager.uiImageDrawRequests.end(), imgComp);
	for (const ImageDrawRequest& image : resourceManager.uiImageDrawRequests) {
		Artist::draw(image);
	}
	resourceManager.uiImageDrawRequests.clear();
	ImageDrawRequest::numRequests = 0;

	// 3. text (Text.Draw)
	for (const TextDrawRequest& text : resourceManager.textDrawRequests) {
		Artist::drawText(text);
	}
	resourceManager.textDrawRequests.clear();
	
	// 4. pixels (Image.DrawPixel)
	for (const PixelDrawRequest& pixel : resourceManager.pixelDrawRequests) {
		Artist::drawPixel(pixel);
	}
	resourceManager.pixelDrawRequests.clear();

}
