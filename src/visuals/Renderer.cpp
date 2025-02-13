// my code
#include "Renderer.h"

// sdl
#include "SDL2/SDL.h"

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
	for (const ImageDrawRequest& image : resourceManager.imageDrawRequests) {
		Artist::draw(image);
	}
	resourceManager.imageDrawRequests.clear();

	// 2. UI images (Image.DrawUI/Image.DrawUIEx)
	// set the render scale to 1
	SDL_RenderSetScale(sdlRenderer, 1, 1);
	for (const ImageDrawRequest& image : resourceManager.uiImageDrawRequests) {
		Artist::draw(image);
	}
	resourceManager.uiImageDrawRequests.clear();
	ImageDrawRequest::numRequests = 0;

	// 3. text (Text.Draw)
	for (const TextDrawRequest& text : resourceManager.textDrawRequests) {
		Artist::drawText(text);
		SDL_DestroyTexture(text.texture); // TODO: cache text textures in the future? might be good, memory tradeoff?
	}
	resourceManager.textDrawRequests.clear();
	
	// 4. pixels (Image.DrawPixel)
	for (const PixelDrawRequest& pixel : resourceManager.pixelDrawRequests) {
		Artist::drawPixel(pixel);
	}
	resourceManager.pixelDrawRequests.clear();

}
